#include "System/PaintTool/ColorBar/ColorBarEntity.hpp"
#include "ColorBarLevel.hpp"
#include "ColorBarManager.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "glm/fwd.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.5f, 0.5f * 0.311f * 2.0f);
    static const glm::vec2 PANEL_OFFSET = glm::vec2(0.375f, -0.175f);
    static const glm::vec2 BAR_SIZE = glm::vec2(0.5f, 0.5f * 0.311f);
    static const uint32_t PLANET_LAYERS_COUNT = 4;
    static const uint32_t GALAXY_LAYERS_COUNT = 4;
    static const glm::vec2 COLOR_LAYER_SIZE = glm::vec2(0.071f, 0.06f);
    static const glm::vec2 DISPLAY_SIZE = glm::vec2(0.1f, 0.1f);
    static const glm::vec2 DISPLAY_OFFSET = glm::vec2(0.08f, -0.075f);
    static const float SELECT_BORDER_THICKNESS = 0.009f;
    static const glm::vec4 SELECT_COLOR = glm::vec4(1.0f, 0.08f, 0.005f, 1);

    static const std::array<glm::vec4, PLANET_LAYERS_COUNT>
        PLANET_COLORS = {
            glm::vec4(0.84f, 0.65f, 0.02f, 1),
            glm::vec4(0.22f, 0.22f, 0.76f, 1),
            glm::vec4(0.62f, 0.14f, 0.45f, 1),
            glm::vec4(0.73f, 0.37f, 0.65f, 1)};

    static const std::array<glm::vec4, PLANET_LAYERS_COUNT> GALAXY_COLORS = {
        glm::vec4(0.3f, 0.8f, 0.3f, 1),
        glm::vec4(0.2f, 0.4f, 0.7f, 1),
        glm::vec4(0.2f, 0.7f, 0.7f, 1),
        glm::vec4(0.3f, 0.4f, 0.8f, 1)};

    static const glm::vec2 HELP_BUTTON_SCALE = glm::vec2(0.05f);
    static const glm::vec2 HELP_BUTTON_OFFSET = glm::vec2(0.05f, 0);
    static const glm::vec2 HELP_POPUP_SCALE = glm::vec2(0.8f, 0.45f);
    static const glm::vec2 HELP_POPUP_OFFSET = glm::vec2(0.0f, 0.05f);
    static const std::string HELP_TEXT = "This is your Color Palette! The colors you capture here will change the look of your Galaxy. Select a color in your palette, then use the Eye Dropper on the canvas above to capture your painted colors. The top row changes the colors of the Galaxy Objects you place. The bottom row changes the Sun and Stardust colors.";

    ColorBarEntity::ColorBarEntity(Function<MouseInput> getMouseInput,
        Function<void, glm::vec4> setColorDisplayColor,
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor,
        Function<std::array<glm::vec4, 4>> getGalaxyColors,
        Function<Rendering::Texture2D*> getBrushTexture,
        Function<Rendering::Texture2D*> getBrushMask,
        Function<void> pickerSelectColor,
        BeerEvent<void(glm::vec4)>* onColorPicked,
        BeerEvent<void()>* onColorPickerClosed,
        BeerEvent<void()>* onNewSeed)
        : getMouseInput(getMouseInput)
        , setColorDisplayColor(setColorDisplayColor)
        , setGalaxyBufferColor(setGalaxyBufferColor)
        , getGalaxyColors(getGalaxyColors)
        , getBrushTexture(getBrushTexture)
        , getBrushMask(getBrushMask)
        , pickerSelectColor(pickerSelectColor)
        , onColorPicked(onColorPicked)
        , onColorPickerClosed(onColorPickerClosed)
        , onNewSeed(onNewSeed)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        markQuadTreeDirty = [this]() -> void { MarkDirty(); };

        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/ColorBar/Tex_ColorBarContainer");

        backgroundMat = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMat->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        backgroundMat->SetTexture("_SpriteTex", backgroundTexture.get());
        backgroundMat->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        rootTransform.Anchor = AnchorMode::MiddleLeft;
        rootTransform.Pivot = AnchorMode::MiddleLeft;
        rootTransform.Position = PANEL_OFFSET;
        rootTransform.Scale = PANEL_SIZE;
        rootTransform.Depth = 0.25f;
        MarkDirty();
    }

    void ColorBarEntity::InitializeColorLayers()
    {
        InitializeDisplays();
        InitializeBackgrounds();
        InitializeSelectSpriteEntity();

        colorLayerSprite = std::make_shared<Rendering::Texture2D>("UI/ColorBar/Tex_ColorLayer");
        glm::vec2 startPos = glm::vec2(0.06f, 0.05f);

        UITransform colorLayersTransform{};
        colorLayersTransform.Position = startPos;
        colorLayersTransform.Scale = COLOR_LAYER_SIZE;
        colorLayersTransform.Anchor = AnchorMode::MiddleLeft;
        colorLayersTransform.Pivot = AnchorMode::BottomRight;
        colorLayersTransform.Depth = 0.275f;

        colorLayers.reserve(PLANET_LAYERS_COUNT);
        colorLayerMaterials.reserve(PLANET_LAYERS_COUNT);

        for (int i = 0; i < PLANET_LAYERS_COUNT; i++)
        {
            colorLayerMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            colorLayerMaterials[i]->SetColor("_TintColor", PLANET_COLORS[i]);
            colorLayerMaterials[i]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

            colorLayersTransform.Position.x += COLOR_LAYER_SIZE.x * 1.05f;
            colorLayers.emplace_back(std::make_unique<UISubEntity>(colorLayersTransform));
            rootTransform.BindChild(colorLayers[i]->GetTransform());
        }

        startPos.y = -0.055f;
        colorLayersTransform.Position = startPos;
        colorLayersTransform.Pivot = AnchorMode::TopRight;

        for (int i = 0; i < GALAXY_LAYERS_COUNT; i++)
        {
            colorLayerMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            colorLayerMaterials[i + PLANET_LAYERS_COUNT]->SetColor("_TintColor", GALAXY_COLORS[i]);
            colorLayerMaterials[i + PLANET_LAYERS_COUNT]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

            colorLayersTransform.Position.x += COLOR_LAYER_SIZE.x * 1.05f;
            colorLayers.emplace_back(std::make_unique<UISubEntity>(colorLayersTransform));
            rootTransform.BindChild(colorLayers[i + PLANET_LAYERS_COUNT]->GetTransform());
        }

        ColorBarManager* colorBarManager = GetColorBarManager();

        for (int i = static_cast<int>(PLANET_LAYERS_COUNT) - 1; i >= 0; i--)
        {
            ColorBarLevel level = static_cast<ColorBarLevel>(i);

            colorBarManager->CreateColorBarController(level,
                ColorBarType::Planet,
                colorLayers[i]->GetTransform(),
                colorLayerMaterials[i].get(),
                PLANET_COLORS[i]);
        }

        for (int i = static_cast<int>(GALAXY_LAYERS_COUNT) - 1; i >= 0; i--)
        {
            ColorBarLevel level = static_cast<ColorBarLevel>(i);

            colorBarManager->CreateColorBarController(level,
                ColorBarType::Galaxy,
                colorLayers[i + PLANET_LAYERS_COUNT]->GetTransform(),
                colorLayerMaterials[i + PLANET_LAYERS_COUNT].get(),
                GALAXY_COLORS[i]);
        }

        colorBarManager->ForceSetColorsFromSeed();
        colorBarManager->UpdateDisplayMaterials();
    }

    void ColorBarEntity::InitializeDisplays()
    {
        glm::vec2 planetDisplayOffset = glm::vec2(0.095f + (PLANET_LAYERS_COUNT + 1) * (COLOR_LAYER_SIZE.x * 1.05f), 0.0315f);

        UITransform displayTransform{};
        displayTransform.Anchor = AnchorMode::MiddleLeft;
        displayTransform.Pivot = AnchorMode::BottomRight;
        displayTransform.Scale = DISPLAY_SIZE;
        displayTransform.Position = planetDisplayOffset;
        displayTransform.Depth = 0.3f;

        planetDisplayEntity = std::make_unique<UISubEntity>(displayTransform);
        rootTransform.BindChild(planetDisplayEntity->GetTransform());

        planetDisplayMaterial = std::make_shared<Rendering::Material>("UI/GalaxyComponentSprite");
        planetDisplayMaterial->SetTexture("_SpriteTex", getBrushTexture());
        planetDisplayMaterial->SetInt("_OverrideMapClip", 1);

        glm::vec2 galaxyDisplayOffset = glm::vec2(0.095f + (GALAXY_LAYERS_COUNT + 1) * (COLOR_LAYER_SIZE.x * 1.05f), -0.0315f);

        displayTransform.Pivot = AnchorMode::TopRight;
        displayTransform.Position = galaxyDisplayOffset;

        galaxyDisplayEntity = std::make_unique<UISubEntity>(displayTransform);
        rootTransform.BindChild(galaxyDisplayEntity->GetTransform());

        galaxyDisplayTexture = std::make_shared<Rendering::Texture2D>("UI/ColorBar/Tex_GalaxyDisplay");
        galaxyDisplayMask = std::make_shared<Rendering::Texture2D>("UI/ColorBar/Tex_GalaxyMask");
        galaxyDisplayMaterial = std::make_shared<Rendering::Material>("UI/GalaxyComponentSprite");
        galaxyDisplayMaterial->SetTexture("_SpriteTex", galaxyDisplayTexture.get());
        galaxyDisplayMaterial->SetTexture("_ColorMask", galaxyDisplayMask.get());
        galaxyDisplayMaterial->SetInt("_OverrideMapClip", 1);

        ColorBarManager* colorBarManager = GetColorBarManager();
        colorBarManager->SetDisplayMaterials(planetDisplayMaterial.get(),
            galaxyDisplayMaterial.get());
    }

    void ColorBarEntity::InitializeBackgrounds()
    {
        UITransform backgroundTransform{};
        backgroundTransform.Anchor = AnchorMode::MiddleLeft;
        backgroundTransform.Pivot = AnchorMode::BottomLeft;
        backgroundTransform.Position = glm::vec2(0, 0.005f);
        backgroundTransform.Depth = 0.4f;

        glm::vec2 planetBgScale = BAR_SIZE;
        backgroundTransform.Scale = planetBgScale;

        planetBarBgEntity = std::make_unique<UISubEntity>(backgroundTransform);
        rootTransform.BindChild(planetBarBgEntity->GetTransform());

        glm::vec2 galaxyBGScale = BAR_SIZE;
        backgroundTransform.Pivot = AnchorMode::TopLeft;
        backgroundTransform.Position = glm::vec2(0, -0.005f);
        backgroundTransform.Scale = galaxyBGScale;

        galaxyBarBgEntity = std::make_unique<UISubEntity>(backgroundTransform);
        rootTransform.BindChild(galaxyBarBgEntity->GetTransform());
    }

    void ColorBarEntity::InitializeSelectSpriteEntity()
    {
        UITransform selectTransform{};
        selectTransform.Anchor = AnchorMode::Center;
        selectTransform.Pivot = AnchorMode::Center;
        selectTransform.Scale = COLOR_LAYER_SIZE + SELECT_BORDER_THICKNESS;
        selectTransform.Depth = 0.6f;
        selectTransform.Position.x = -0.01f;

        selectSpriteEntity = std::make_unique<UISubEntity>(selectTransform);

        selectSpriteTexture = std::make_shared<Rendering::Texture2D>("UI/ColorBar/Tex_SelectBorder");
        selectSpriteMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        selectSpriteMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));
        selectSpriteMaterial->SetColor("_TintColor", SELECT_COLOR);
        selectSpriteMaterial->SetTexture("_SpriteTex", selectSpriteTexture.get());

        GetColorBarManager()->SetSelectSpriteTransform(selectSpriteEntity->GetTransform());
    }
} // namespace Beer::System
