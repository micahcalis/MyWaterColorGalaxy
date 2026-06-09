#include "ColorMixerEntity.hpp"
#include "ColorMixerManager.hpp"
#include "PigmentButton.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "glm/fwd.hpp"
#include <array>

namespace Beer::System
{
    static const uint32_t NUM_PIGMENTS = 10;
    static const uint32_t NUM_ROWS = 2;

    static const std::array<PigmentType, NUM_PIGMENTS> BUTTON_PIGMENTS = {
        PigmentType::QuinacridoneRose,
        PigmentType::CadmiumRed,
        PigmentType::HansaYellow,
        PigmentType::BrilliantOrange,
        PigmentType::FrenchUltramarine,
        PigmentType::CeruleanBlue,
        PigmentType::PhthaloGreen,
        PigmentType::HookersGreen,
        PigmentType::BurntUmber,
        PigmentType::IndianRed};

    static const float DISPLAY_SCALE = 0.75f;
    static const glm::vec2 DISPLAY_OFFSET = glm::vec2(0.22f, -0.35f);
    static const float PIGMENT_BUTTON_SCALE = 0.15f;
    static const float CLEAR_BUTTON_SCALE = 0.2f;
    static const float PICKER_BUTTON_SCALE = 0.2f;
    static const float PAINT_BUTTON_SCALE = 0.2f;
    static const float COLOR_DISPLAY_SCALE = 0.2f;
    static const float CLOSE_BUTTON_SCALE = 0.1f;

    ColorMixerEntity::ColorMixerEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        colorMixerDisplayMat = std::make_shared<Rendering::Material>("UI/ColorMixerSprite");
        colorMixerDisplayMat->SetColor("_CanvasColor", glm::vec4(0.969f, 0.969f, 0.914, 1));

        rootTransform.Anchor = AnchorMode::MiddleLeft;
        rootTransform.Pivot = AnchorMode::MiddleLeft;
        rootTransform.Scale = glm::vec2(DISPLAY_SCALE);
        rootTransform.Position = DISPLAY_OFFSET;
        rootTransform.Depth = 0.1f;
        MarkDirty();
    }

    void ColorMixerEntity::InitializePigments()
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        uint32_t rowCount = NUM_PIGMENTS / NUM_ROWS;
        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        circleTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_CircleSprite");
        pigmentEntities.reserve(NUM_PIGMENTS);
        pigmentMaterials.reserve(NUM_PIGMENTS);

        for (int i = 0; i < NUM_PIGMENTS; i++)
        {
            UITransform transform{};
            transform.Anchor = AnchorMode::TopRight;
            transform.Pivot = AnchorMode::BottomRight;
            transform.Scale = glm::vec2(PIGMENT_BUTTON_SCALE, PIGMENT_BUTTON_SCALE);

            float offsetX = static_cast<float>(i % rowCount);
            float offsetY = static_cast<float>(i / rowCount);
            transform.Position = glm::vec2(-offsetX * PIGMENT_BUTTON_SCALE, offsetY * PIGMENT_BUTTON_SCALE);

            pigmentEntities.emplace_back(std::make_unique<UISubEntity>(transform));
            rootTransform.BindChild(pigmentEntities[i]->GetTransform());

            PigmentType type = BUTTON_PIGMENTS[i];

            std::shared_ptr<Rendering::Material> material = std::make_shared<Rendering::Material>("UI/SpriteDefault");
            material->SetColor("_TintColor", GetPigmentColor(type));
            material->SetTexture("_SpriteTex", squareTexture.get());
            material->SetVector("_Scale", glm::vec4(1, 1, 0, 0));
            pigmentMaterials.push_back(std::move(material));

            mixerManager->AddPigmentButton(pigmentEntities[i]->GetTransform(),
                pigmentMaterials[i].get(),
                type);
        }

        MarkDirty();
    }

    void ColorMixerEntity::InitializeClearButton(Function<void> markCanvasClear)
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        UITransform clearTransform{};
        clearTransform.Scale = glm::vec2(CLEAR_BUTTON_SCALE, CLEAR_BUTTON_SCALE);
        clearTransform.Pivot = AnchorMode::BottomRight;
        clearTransform.Anchor = AnchorMode::BottomLeft;

        clearButtonIcon = std::make_unique<UISubEntity>(clearTransform);
        rootTransform.BindChild(clearButtonIcon->GetTransform());

        clearIconTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_GarbageIcon");
        clearIconMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        clearIconMaterial->SetColor("_TintColor", glm::vec4(1.0f, 1.0f, 1.0f, 1));
        clearIconMaterial->SetTexture("_SpriteTex", clearIconTexture.get());
        clearIconMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        mixerManager->SetClearButton(markCanvasClear,
            clearButtonIcon->GetTransform(),
            clearIconMaterial.get());

        MarkDirty();
    }

    void ColorMixerEntity::InitializeColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
        Function<MouseInput> getMouseInput)
    {
        ColorMixerManager* mixerManager = GetMixerManager();
        UITransform colorPickerTransform{};
        colorPickerTransform.Scale = glm::vec2(PICKER_BUTTON_SCALE, PICKER_BUTTON_SCALE);
        colorPickerTransform.Pivot = AnchorMode::BottomRight;
        colorPickerTransform.Anchor = AnchorMode::BottomLeft;
        colorPickerTransform.Position = glm::vec2(0, CLEAR_BUTTON_SCALE);

        colorPickerIcon = std::make_unique<UISubEntity>(colorPickerTransform);
        rootTransform.BindChild(colorPickerIcon->GetTransform());

        colorPickerIconTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_ColorPickIcon");
        colorPickerIconMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        colorPickerIconMaterial->SetColor("_TintColor", glm::vec4(1.0f, 1.0f, 1.0f, 1));
        colorPickerIconMaterial->SetTexture("_SpriteTex", colorPickerIconTexture.get());
        colorPickerIconMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        UITransform paintPigmentTransform{};
        paintPigmentTransform.Scale = glm::vec2(PAINT_BUTTON_SCALE, PAINT_BUTTON_SCALE);
        paintPigmentTransform.Pivot = AnchorMode::BottomRight;
        paintPigmentTransform.Anchor = AnchorMode::BottomLeft;
        paintPigmentTransform.Position = glm::vec2(0, CLEAR_BUTTON_SCALE + PICKER_BUTTON_SCALE);

        paintPigmentIcon = std::make_unique<UISubEntity>(paintPigmentTransform);
        rootTransform.BindChild(paintPigmentIcon->GetTransform());

        paintPigmentIconTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_PaintIcon");
        paintPigmentIconMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        paintPigmentIconMaterial->SetColor("_TintColor", glm::vec4(1));
        paintPigmentIconMaterial->SetTexture("_SpriteTex", paintPigmentIconTexture.get());
        paintPigmentIconMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        UITransform displayTransform{};
        displayTransform.Scale = glm::vec2(COLOR_DISPLAY_SCALE, COLOR_DISPLAY_SCALE);
        displayTransform.Pivot = AnchorMode::BottomRight;
        displayTransform.Anchor = AnchorMode::BottomLeft;
        displayTransform.Position = glm::vec2(0, CLEAR_BUTTON_SCALE + PICKER_BUTTON_SCALE + PAINT_BUTTON_SCALE);

        colorDisplay = std::make_unique<UISubEntity>(displayTransform);
        rootTransform.BindChild(colorDisplay->GetTransform());

        colorDisplayMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        colorDisplayMaterial->SetColor("_TintColor", glm::vec4(1));
        colorDisplayMaterial->SetTexture("_SpriteTex", circleTexture.get());
        colorDisplayMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        mixerManager->SetColorPicker(subscribeToReadback,
            getMouseInput,
            colorPickerIcon->GetTransform(),
            colorPickerIconMaterial.get(),
            paintPigmentIcon->GetTransform(),
            paintPigmentIconMaterial.get(),
            &rootTransform);

        mixerManager->GetColorPicker()->OnColorPicked.Subscribe([this](glm::vec4 color) -> void {
            colorDisplayMaterial->SetColor("_TintColor", color);
        });

        MarkDirty();
    }
} // namespace Beer::System
