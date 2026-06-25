#include "ColorMixerEntity.hpp"
#include "ColorMixerManager.hpp"
#include "PigmentButton.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"
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

    static const std::array<std::string, NUM_PIGMENTS> PIGMENT_PATHS = {
        "UI/ColorMixer/Tex_PaintBlob1",
        "UI/ColorMixer/Tex_PaintBlob2",
        "UI/ColorMixer/Tex_PaintBlob3",
        "UI/ColorMixer/Tex_PaintBlob4",
        "UI/ColorMixer/Tex_PaintBlob5",
        "UI/ColorMixer/Tex_PaintBlob6",
        "UI/ColorMixer/Tex_PaintBlob7",
        "UI/ColorMixer/Tex_PaintBlob8",
        "UI/ColorMixer/Tex_PaintBlob9",
        "UI/ColorMixer/Tex_PaintBlob10",
    };

    static const std::array<glm::vec2, NUM_PIGMENTS> PIGMENT_POSITIONS = {
        glm::vec2(-0.35f, -0.15f),
        glm::vec2(-0.18f, -0.28f),
        glm::vec2(0.02f, -0.32f),
        glm::vec2(0.22f, -0.25f),
        glm::vec2(0.33f, -0.06f),
        glm::vec2(0.36f, 0.14f),
        glm::vec2(0.23f, 0.3f),
        glm::vec2(0.0f, -0.1f),
        glm::vec2(0.16f, 0.05f),
        glm::vec2(0.05f, 0.20f)};

    static const glm::vec2 DISPLAY_SCALE = glm::vec2(0.9f, 0.9f * 1.4f);
    static const glm::vec2 DISPLAY_OFFSET = glm::vec2(0.18f, -0.05f);
    static const float COLOR_MIXER_SCALE = 0.5f;
    static const glm::vec2 COLOR_MIXER_OFFSET = glm::vec2(0.025f, 0.325f);
    static const glm::vec2 COLOR_MIXER_BORDER_ADD = glm::vec2(0.05f);
    static const float PIGMENT_BUTTON_SCALE = 0.08f;
    static const glm::vec2 CLEAR_BUTTON_SCALE = glm::vec2(0.15f * 0.75f, 0.15f);
    static const glm::vec2 CLEAR_BUTTON_OFFSET = glm::vec2(0.17f, 0.7f);
    static const float PICKER_BUTTON_SCALE = 0.2f;
    static const float PAINT_BUTTON_SCALE = 0.2f;
    static const float COLOR_DISPLAY_SCALE = 0.2f;
    static const float CLOSE_BUTTON_SCALE = 0.1f;
    static const glm::vec2 PIGMENT_BG_SCALE = glm::vec2(0.5f, 0.5f * 0.845f);
    static const glm::vec2 PIGMENT_BG_POS = glm::vec2(0.15f, 0.0f);

    static const float SELECT_BORDER_THICKNESS = 0.02f;
    static const glm::vec4 SELECT_COLOR = glm::vec4(1.0f, 0.08f, 0.005f, 1);

    static const glm::vec2 CURSOR_SIZE = glm::vec2(0.1f);
    static const glm::vec2 CURSOR_OFFSET = glm::vec2(-0.025f, -0.025f);

    static const glm::vec2 HELP_BUTTON_SCALE = glm::vec2(0.05f);
    static const glm::vec2 HELP_BUTTON_OFFSET = glm::vec2(0.05f, 0);
    static const glm::vec2 HELP_POPUP_SCALE = glm::vec2(0.75f, 0.4f);
    static const glm::vec2 HELP_POPUP_OFFSET = glm::vec2(0.0f, 0.05f);
    static const std::string HELP_TEXT = "This is your Color Mixing Canvas! Select a pigment and then paint on the canvas. Mix different pigments for new color possibilities. You can clear the canvas using the bin icon. When you are happy with the colors, you can capture them on your palette below.";

    static const glm::vec2 COLOR_DISPLAY_DIM = glm::vec2(0.15f, 0.45f);
    static const glm::vec2 COLOR_DISPLAY_OFFSET = glm::vec2(-0.16f, 0.285f);

    ColorMixerEntity::ColorMixerEntity(Function<MouseInput> getMouseInput)
        : getMouseInput(getMouseInput)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        colorMixerDisplayTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_MixerPaper");
        colorMixerDisplayMat = std::make_shared<Rendering::Material>("UI/ColorMixerSprite");
        colorMixerDisplayMat->SetColor("_CanvasColor", Rendering::CANVAS_COLOR);
        colorMixerDisplayMat->SetTexture("_SpriteTex", colorMixerDisplayTexture.get());

        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_ToolFrame");
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetTexture("_SpriteTex", backgroundTexture.get());
        backgroundMaterial->SetColor("_TintColor", glm::vec4(1));
        backgroundMaterial->SetVector("_Scale", glm::vec4(1));

        rootTransform.Anchor = AnchorMode::MiddleLeft;
        rootTransform.Pivot = AnchorMode::MiddleLeft;
        rootTransform.Scale = glm::vec2(DISPLAY_SCALE);
        rootTransform.Position = DISPLAY_OFFSET;

        UITransform mixerTransform{};
        mixerTransform.Anchor = AnchorMode::Center;
        mixerTransform.Pivot = AnchorMode::Center;
        mixerTransform.Scale = glm::vec2(COLOR_MIXER_SCALE);
        mixerTransform.Position = COLOR_MIXER_OFFSET;
        mixerTransform.Depth = 0.1f;

        colorMixerDisplayEntity = std::make_unique<UISubEntity>(mixerTransform);
        rootTransform.BindChild(colorMixerDisplayEntity->GetTransform());

        colorMixerBorderTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_MixerBorder");
        colorMixerBorderMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        colorMixerBorderMaterial->SetTexture("_SpriteTex", colorMixerBorderTexture.get());
        colorMixerBorderMaterial->SetColor("_TintColor", glm::vec4(1));
        colorMixerBorderMaterial->SetVector("_Scale", glm::vec4(1));

        mixerTransform.Anchor = AnchorMode::Center;
        mixerTransform.Pivot = AnchorMode::Center;
        mixerTransform.Depth += 0.1f;
        mixerTransform.Position = glm::vec2(0);
        mixerTransform.Scale += COLOR_MIXER_BORDER_ADD;

        colormixerBorderEntity = std::make_unique<UISubEntity>(mixerTransform);
        colorMixerDisplayEntity->GetTransform()->BindChild(colormixerBorderEntity->GetTransform());

        MarkDirty();
    }

    void ColorMixerEntity::InitializePigments()
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        UITransform bgTransform{};
        bgTransform.Anchor = AnchorMode::TopLeft;
        bgTransform.Pivot = AnchorMode::Center;
        bgTransform.Scale = PIGMENT_BG_SCALE;
        bgTransform.Position = PIGMENT_BG_POS;
        bgTransform.Depth = 0.3f;

        pigmentsBgEntity = std::make_unique<UISubEntity>(bgTransform);
        rootTransform.BindChild(pigmentsBgEntity->GetTransform());

        pigmentsBgTex = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_Palette");

        pigmentsBgMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        pigmentsBgMaterial->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        pigmentsBgMaterial->SetTexture("_SpriteTex", pigmentsBgTex.get());
        pigmentsBgMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        uint32_t rowCount = NUM_PIGMENTS / NUM_ROWS;
        pigmentEntities.reserve(NUM_PIGMENTS);
        pigmentMaterials.reserve(NUM_PIGMENTS);

        for (int i = 0; i < NUM_PIGMENTS; i++)
        {
            UITransform transform{};
            transform.Anchor = AnchorMode::Center;
            transform.Pivot = AnchorMode::Center;
            transform.Scale = glm::vec2(PIGMENT_BUTTON_SCALE, PIGMENT_BUTTON_SCALE);
            transform.Depth = 0.35f;

            transform.Position = PIGMENT_POSITIONS[i] * PIGMENT_BG_SCALE;

            pigmentEntities.emplace_back(std::make_unique<UISubEntity>(transform));
            pigmentsBgEntity->GetTransform()->BindChild(pigmentEntities[i]->GetTransform());

            PigmentType type = BUTTON_PIGMENTS[i];

            std::shared_ptr<Rendering::Material> material = std::make_shared<Rendering::Material>("UI/SpriteDefault");
            std::shared_ptr<Rendering::Texture2D> texture = std::make_shared<Rendering::Texture2D>(PIGMENT_PATHS[i]);
            material->SetColor("_TintColor", GetPigmentColor(type));
            material->SetTexture("_SpriteTex", texture.get());
            material->SetVector("_Scale", glm::vec4(1, 1, 0, 0));
            pigmentMaterials.push_back(std::move(material));
            pigmentTextures.push_back(std::move(texture));

            mixerManager->AddPigmentButton(pigmentEntities[i]->GetTransform(),
                pigmentMaterials[i].get(),
                type);
        }

        InitializeSelectSpriteEntity();
        InitializeHelpButton();
        InitializeColorDisplay();

        MarkDirty();
    }

    void ColorMixerEntity::InitializeClearButton(Function<void> markCanvasClear)
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        UITransform clearTransform{};
        clearTransform.Scale = CLEAR_BUTTON_SCALE;
        clearTransform.Pivot = AnchorMode::BottomRight;
        clearTransform.Anchor = AnchorMode::BottomLeft;
        clearTransform.Position = CLEAR_BUTTON_OFFSET;

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
        displayTransform.Position = glm::vec2(0, CLEAR_BUTTON_SCALE.x + PICKER_BUTTON_SCALE + PAINT_BUTTON_SCALE);

        mixerManager->SetColorPicker(subscribeToReadback,
            getMouseInput,
            colorMixerDisplayEntity->GetTransform());

        InitializeCursorSprite();

        MarkDirty();
    }

    void ColorMixerEntity::InitializeSelectSpriteEntity()
    {
        UITransform selectTransform{};
        selectTransform.Anchor = AnchorMode::Center;
        selectTransform.Pivot = AnchorMode::Center;
        selectTransform.Scale = glm::vec2(PIGMENT_BUTTON_SCALE + SELECT_BORDER_THICKNESS);
        selectTransform.Depth = 0.4f;

        selectSpriteEntity = std::make_unique<UISubEntity>(selectTransform);

        selectSpriteTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_SelectSprite");
        selectSpriteMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        selectSpriteMaterial->SetTexture("_SpriteTex", selectSpriteTexture.get());
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
        colorMixerDisplayEntity->GetTransform()->BindChild(cursorAnchorEntity->GetTransform());

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

        GetMixerManager()->SetColorMixerCursor(colorMixerDisplayEntity->GetTransform(),
            cursorAnchorEntity->GetTransform(),
            cursorSpriteMaterial.get(),
            brushSprite.get(),
            brushSpriteMask.get(),
            pickerSprite.get(),
            pickerSpriteMask.get());
    }

    void ColorMixerEntity::InitializeHelpButton()
    {
        UITransform helpButtonTransform{};
        helpButtonTransform.Anchor = AnchorMode::TopRight;
        helpButtonTransform.Pivot = AnchorMode::TopLeft;
        helpButtonTransform.Scale = HELP_BUTTON_SCALE;
        helpButtonTransform.Position = HELP_BUTTON_OFFSET;
        helpButtonTransform.Depth = 0.5f;

        UITransform helpPopupTransform{};
        helpPopupTransform.Anchor = AnchorMode::BottomMiddle;
        helpPopupTransform.Pivot = AnchorMode::TopMiddle;
        helpPopupTransform.Scale = HELP_POPUP_SCALE;
        helpPopupTransform.Position = HELP_POPUP_OFFSET;
        helpPopupTransform.Depth = 0.6f;

        helpButtonSubEntity = std::make_unique<HelpButtonSubEntity>(
            &rootTransform,
            helpButtonTransform,
            HELP_TEXT,
            helpPopupTransform);

        GetMixerManager()->SetHelpToggle(helpButtonSubEntity.get(),
            [this]() -> void { MarkDirty(); });
    }

    void ColorMixerEntity::InitializeColorDisplay()
    {
        UITransform colorDisplayTransform{};
        colorDisplayTransform.Anchor = AnchorMode::MiddleRight;
        colorDisplayTransform.Pivot = AnchorMode::MiddleLeft;
        colorDisplayTransform.Scale = COLOR_DISPLAY_DIM;
        colorDisplayTransform.Position = COLOR_DISPLAY_OFFSET;

        colorDisplaySubEntity = std::make_unique<ColorDisplaySubEntity>(colorDisplayTransform,
            &rootTransform);

        GetMixerManager()->InitializeColorDisplay(colorDisplaySubEntity.get());
    }
} // namespace Beer::System
