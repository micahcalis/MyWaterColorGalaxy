#include "ColorMixerEntity.hpp"
#include "ColorMixerManager.hpp"
#include "PigmentButton.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "glm/fwd.hpp"
#include <array>
#include <memory>

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
    static const glm::vec2 DISPLAY_OFFSET = glm::vec2(0.22f, 0.35f);
    static const float PIGMENT_BUTTON_SCALE = 0.15f;
    static const float CLEAR_BUTTON_SCALE = 0.2f;
    static const float PICKER_BUTTON_SCALE = 0.2f;
    static const float PAINT_BUTTON_SCALE = 0.2f;
    static const float COLOR_DISPLAY_SCALE = 0.2f;
    static const float CLOSE_BUTTON_SCALE = 0.1f;
    static const glm::vec2 PIGMENT_BG_SCALE = glm::vec2(0.85f, 0.4f);
    static const glm::vec2 PIGMENT_BG_POS = glm::vec2(0, -0.05f);

    static const float SELECT_BORDER_THICKNESS = 0.02f;
    static const glm::vec4 SELECT_COLOR = glm::vec4(0, 0, 0, 1);

    static const glm::vec2 CURSOR_SIZE = glm::vec2(0.1f);
    static const glm::vec2 CURSOR_OFFSET = glm::vec2(-0.025f, -0.025f);

    ColorMixerEntity::ColorMixerEntity(Function<MouseInput> getMouseInput)
        : getMouseInput(getMouseInput)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        colorMixerDisplayMat = std::make_shared<Rendering::Material>("UI/ColorMixerSprite");
        colorMixerDisplayMat->SetColor("_CanvasColor", Rendering::CANVAS_COLOR);

        rootTransform.Anchor = AnchorMode::MiddleLeft;
        rootTransform.Pivot = AnchorMode::MiddleLeft;
        rootTransform.Scale = glm::vec2(DISPLAY_SCALE);
        rootTransform.Position = DISPLAY_OFFSET;
        rootTransform.Depth = 0.1f;

        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");

        MarkDirty();
    }

    void ColorMixerEntity::InitializePigments()
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        uint32_t rowCount = NUM_PIGMENTS / NUM_ROWS;
        circleTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_CircleSprite");
        pigmentEntities.reserve(NUM_PIGMENTS);
        pigmentMaterials.reserve(NUM_PIGMENTS);

        for (int i = 0; i < NUM_PIGMENTS; i++)
        {
            UITransform transform{};
            transform.Anchor = AnchorMode::TopRight;
            transform.Pivot = AnchorMode::BottomRight;
            transform.Scale = glm::vec2(PIGMENT_BUTTON_SCALE, PIGMENT_BUTTON_SCALE);
            transform.Depth = 0.1f;

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

        UITransform bgTransform{};
        bgTransform.Anchor = AnchorMode::TopMiddle;
        bgTransform.Pivot = AnchorMode::BottomMiddle;
        bgTransform.Scale = PIGMENT_BG_SCALE;
        bgTransform.Position = PIGMENT_BG_POS;

        pigmentsBgEntity = std::make_unique<UISubEntity>(bgTransform);
        rootTransform.BindChild(pigmentsBgEntity->GetTransform());

        pigmentsBgTex = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");

        pigmentsBgMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        pigmentsBgMaterial->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        pigmentsBgMaterial->SetTexture("_SpriteTex", pigmentsBgTex.get());
        pigmentsBgMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        InitializeSelectSpriteEntity();

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

        UITransform displayTransform{};
        displayTransform.Scale = glm::vec2(COLOR_DISPLAY_SCALE, COLOR_DISPLAY_SCALE);
        displayTransform.Pivot = AnchorMode::BottomRight;
        displayTransform.Anchor = AnchorMode::BottomLeft;
        displayTransform.Position = glm::vec2(0, CLEAR_BUTTON_SCALE + PICKER_BUTTON_SCALE + PAINT_BUTTON_SCALE);

        mixerManager->SetColorPicker(subscribeToReadback,
            getMouseInput,
            &rootTransform);

        InitializeCursorSprite();

        MarkDirty();
    }

    void ColorMixerEntity::InitializeSelectSpriteEntity()
    {
        UITransform selectTransform{};
        selectTransform.Anchor = AnchorMode::Center;
        selectTransform.Pivot = AnchorMode::Center;
        selectTransform.Scale = glm::vec2(PIGMENT_BUTTON_SCALE + SELECT_BORDER_THICKNESS);
        selectTransform.Depth = 0.05f;

        selectSpriteEntity = std::make_unique<UISubEntity>(selectTransform);

        selectSpriteMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        selectSpriteMaterial->SetTexture("_SpriteTex", squareTexture.get());
        selectSpriteMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));
        selectSpriteMaterial->SetColor("_TintColor", SELECT_COLOR);

        GetMixerManager()->SetSelectButton(selectSpriteEntity->GetTransform());
    }

    void ColorMixerEntity::InitializeCursorSprite()
    {
        UITransform cursorTransform{};
        cursorTransform.Scale = glm::vec2(0.001f);
        cursorTransform.Anchor = AnchorMode::BottomLeft;
        cursorTransform.Pivot = AnchorMode::Center;

        cursorAnchorEntity = std::make_unique<UISubEntity>(cursorTransform);
        rootTransform.BindChild(cursorAnchorEntity->GetTransform());

        cursorTransform.Scale = CURSOR_SIZE;
        cursorTransform.Position = CURSOR_OFFSET;
        cursorTransform.Pivot = AnchorMode::BottomLeft;
        cursorTransform.Depth = 0.1f;

        cursorSpriteEntity = std::make_unique<UISubEntity>(cursorTransform);
        cursorAnchorEntity->GetTransform()->BindChild(cursorSpriteEntity->GetTransform());

        pickerSprite = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_PickerMouse");
        pickerSpriteMask = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_PickerMouseMask");

        brushSprite = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_BrushMouse");
        brushSpriteMask = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_BrushMouseMask");

        cursorSpriteMaterial = std::make_shared<Rendering::Material>("UI/MixerCursorSprite");

        GetMixerManager()->SetColorMixerCursor(&rootTransform,
            cursorAnchorEntity->GetTransform(),
            cursorSpriteMaterial.get(),
            brushSprite.get(),
            brushSpriteMask.get(),
            pickerSprite.get(),
            pickerSpriteMask.get());
    }
} // namespace Beer::System
