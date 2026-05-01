#include "ColorMixerEntity.hpp"
#include "ColorMixerManager.hpp"
#include "PigmentButton.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "glm/fwd.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::System
{
    const static uint32_t NUM_PIGMENTS = 8;
    const static uint32_t NUM_ROWS = 2;
    const static float PIGMENT_BUTTON_SCALE = 0.2f;
    const static float CLEAR_BUTTON_SCALE = 0.25f;
    const static float PICKER_BUTTON_SCALE = 0.25f;
    const static float COLOR_DISPLAY_SCALE = 0.25f;
    const static float CLOSE_BUTTON_SCALE = 0.1f;

    ColorMixerEntity::ColorMixerEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        colorMixerDisplayMat = std::make_shared<Rendering::Material>("UI/ColorMixerSprite");
        colorMixerDisplayMat->SetColor("_CanvasColor", glm::vec4(0.969f, 0.969f, 0.914, 1));

        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;
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

            std::shared_ptr<Rendering::Material> material = std::make_shared<Rendering::Material>("UI/SpriteDefault");
            material->SetColor("_TintColor", GetPigmentColor(static_cast<PigmentType>(i)));
            material->SetTexture("_SpriteTex", squareTexture.get());
            material->SetVector("_Scale", glm::vec4(1, 1, 0, 0));
            pigmentMaterials.push_back(std::move(material));

            mixerManager->AddPigmentButton(pigmentEntities[i]->GetTransform(),
                pigmentMaterials[i].get(),
                static_cast<PigmentType>(i));
        }

        MarkDirty();
    }

    void ColorMixerEntity::InitializeClearButton(Function<void> markCanvasClear)
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        UITransform baseClearTransform{};
        baseClearTransform.Scale = glm::vec2(CLEAR_BUTTON_SCALE, CLEAR_BUTTON_SCALE);
        baseClearTransform.Pivot = AnchorMode::BottomRight;
        baseClearTransform.Anchor = AnchorMode::BottomLeft;

        clearButtonBg = std::make_unique<UISubEntity>(baseClearTransform);
        rootTransform.BindChild(clearButtonBg->GetTransform());

        UITransform iconClearTransform{};
        iconClearTransform.Scale = glm::vec2(CLEAR_BUTTON_SCALE, CLEAR_BUTTON_SCALE);
        iconClearTransform.Pivot = AnchorMode::Center;
        iconClearTransform.Anchor = AnchorMode::Center;
        iconClearTransform.Depth += 0.01f;

        clearButtonIcon = std::make_unique<UISubEntity>(iconClearTransform);
        clearButtonBg->GetTransform()->BindChild(clearButtonIcon->GetTransform());

        clearBgMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        clearBgMaterial->SetColor("_TintColor", glm::vec4(1));
        clearBgMaterial->SetTexture("_SpriteTex", squareTexture.get());
        clearBgMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        clearIconTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_GarbageIcon");
        clearIconMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        clearIconMaterial->SetColor("_TintColor", glm::vec4(0.5f, 0.5f, 0.5f, 1));
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
        UITransform baseMixerTransform{};
        baseMixerTransform.Scale = glm::vec2(PICKER_BUTTON_SCALE, PICKER_BUTTON_SCALE);
        baseMixerTransform.Pivot = AnchorMode::BottomRight;
        baseMixerTransform.Anchor = AnchorMode::BottomLeft;
        baseMixerTransform.Position = glm::vec2(0, CLEAR_BUTTON_SCALE);

        colorPickerBg = std::make_unique<UISubEntity>(baseMixerTransform);
        rootTransform.BindChild(colorPickerBg->GetTransform());

        UITransform iconPickerTransform{};
        iconPickerTransform.Scale = glm::vec2(PICKER_BUTTON_SCALE, PICKER_BUTTON_SCALE);
        iconPickerTransform.Pivot = AnchorMode::Center;
        iconPickerTransform.Anchor = AnchorMode::Center;
        iconPickerTransform.Depth += 0.01f;

        colorPickerIcon = std::make_unique<UISubEntity>(iconPickerTransform);
        colorPickerBg->GetTransform()->BindChild(colorPickerIcon->GetTransform());

        colorPickerBgMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        colorPickerBgMaterial->SetColor("_TintColor", glm::vec4(1));
        colorPickerBgMaterial->SetTexture("_SpriteTex", squareTexture.get());
        colorPickerBgMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        colorPickerIconTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_ColorPickIcon");
        colorPickerIconMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        colorPickerIconMaterial->SetColor("_TintColor", glm::vec4(0.5f, 0.5f, 0.5f, 1));
        colorPickerIconMaterial->SetTexture("_SpriteTex", colorPickerIconTexture.get());
        colorPickerIconMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        UITransform displayTransform{};
        displayTransform.Scale = glm::vec2(COLOR_DISPLAY_SCALE, COLOR_DISPLAY_SCALE);
        displayTransform.Pivot = AnchorMode::BottomRight;
        displayTransform.Anchor = AnchorMode::BottomLeft;
        displayTransform.Position = glm::vec2(0, CLEAR_BUTTON_SCALE + PICKER_BUTTON_SCALE);

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
            &rootTransform);

        mixerManager->GetColorPicker()->OnColorPicked.Subscribe([this](glm::vec4 color) -> void {
            colorDisplayMaterial->SetColor("_TintColor", color);
        });

        MarkDirty();
    }

    void ColorMixerEntity::InitializeCloseButton()
    {
        ColorMixerManager* mixerManager = GetMixerManager();
        UITransform closeButtonTransform{};
        closeButtonTransform.Anchor = AnchorMode::TopRight;
        closeButtonTransform.Pivot = AnchorMode::BottomLeft;
        closeButtonTransform.Scale = glm::vec2(CLOSE_BUTTON_SCALE, CLOSE_BUTTON_SCALE);

        closeButton = std::make_unique<UISubEntity>(closeButtonTransform);
        rootTransform.BindChild(closeButton->GetTransform());

        closeButtonTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_CloseButton");
        closeButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        closeButtonMaterial->SetColor("_TintColor", glm::vec4(1));
        closeButtonMaterial->SetTexture("_SpriteTex", closeButtonTexture.get());
        closeButtonMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        mixerManager->SetCloseButton([this]() -> void { Close(); },
            closeButton->GetTransform(),
            closeButtonMaterial.get());
    }
} // namespace Beer::System
