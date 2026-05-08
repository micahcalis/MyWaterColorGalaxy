#include "System/PaintTool/ColorBar/ColorBarEntity.hpp"
#include "ColorBarLevel.hpp"
#include "ColorBarManager.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "glm/fwd.hpp"

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.8f, 0.8f);
    static const uint32_t PLANET_LAYERS_COUNT = 4;
    static const uint32_t GALAXY_LAYERS_COUNT = 4;
    static const glm::vec2 COLOR_LAYER_SIZE = glm::vec2(0.3f, 0.1f);

    static const std::array<glm::vec4, PLANET_LAYERS_COUNT> PLANET_COLORS = {
        glm::vec4(0.8f, 0.2f, 0.1f, 1),
        glm::vec4(0.6f, 0.1f, 0.6f, 1),
        glm::vec4(0.2f, 0.7f, 0.7f, 1),
        glm::vec4(0.3f, 0.4f, 0.8f, 1)};

    static const std::array<glm::vec4, PLANET_LAYERS_COUNT> GALAXY_COLORS = {
        glm::vec4(0.3f, 0.8f, 0.3f, 1),
        glm::vec4(0.2f, 0.4f, 0.7f, 1),
        glm::vec4(0.2f, 0.7f, 0.7f, 1),
        glm::vec4(0.3f, 0.4f, 0.8f, 1)};

    ColorBarEntity::ColorBarEntity(Function<MouseInput> getMouseInput,
        Function<void> openColorPicker,
        Function<void, glm::vec4> setColorDisplayColor,
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor,
        Function<std::array<glm::vec4, 4>> getGalaxyColors,
        BeerEvent<void(glm::vec4)>* onColorPicked,
        BeerEvent<void()>* onColorPickerClosed,
        BeerEvent<void()>* onNewSeed)
        : getMouseInput(getMouseInput), openColorPicker(openColorPicker), setColorDisplayColor(setColorDisplayColor), setGalaxyBufferColor(setGalaxyBufferColor), getGalaxyColors(getGalaxyColors), onColorPicked(onColorPicked), onColorPickerClosed(onColorPickerClosed), onNewSeed(onNewSeed), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        markQuadTreeDirty = [this]() -> void { MarkDirty(); };

        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");

        backgroundMat = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMat->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        backgroundMat->SetTexture("_SpriteTex", squareTexture.get());
        backgroundMat->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        rootTransform.Anchor = AnchorMode::TopLeft;
        rootTransform.Pivot = AnchorMode::TopLeft;
        rootTransform.Position.x += 0.02f;
        rootTransform.Scale = PANEL_SIZE;
        MarkDirty();
    }

    void ColorBarEntity::InitializeColorLayers()
    {
        colorLayerSprite = std::make_shared<Rendering::Texture2D>("UI/ColorBar/Tex_ColorLayer");
        glm::vec2 startPos = glm::vec2(0, 0.1f);

        UITransform colorLayersTransform{};
        colorLayersTransform.Position = startPos;
        colorLayersTransform.Scale = COLOR_LAYER_SIZE;
        colorLayersTransform.Anchor = AnchorMode::BottomMiddle;
        colorLayersTransform.Pivot = AnchorMode::BottomRight;

        colorLayers.reserve(PLANET_LAYERS_COUNT);
        colorLayerMaterials.reserve(PLANET_LAYERS_COUNT);

        for (int i = 0; i < PLANET_LAYERS_COUNT; i++)
        {
            colorLayerMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            colorLayerMaterials[i]->SetColor("_TintColor", PLANET_COLORS[i]);
            colorLayerMaterials[i]->SetTexture("_SpriteTex", colorLayerSprite.get());
            colorLayerMaterials[i]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

            colorLayersTransform.Position.y += COLOR_LAYER_SIZE.y * 0.75f;
            colorLayers.emplace_back(std::make_unique<UISubEntity>(colorLayersTransform));
            rootTransform.BindChild(colorLayers[i]->GetTransform());
        }

        colorLayersTransform.Position = startPos;
        colorLayersTransform.Pivot = AnchorMode::BottomLeft;

        for (int i = 0; i < GALAXY_LAYERS_COUNT; i++)
        {
            colorLayerMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            colorLayerMaterials[i + PLANET_LAYERS_COUNT]->SetColor("_TintColor", GALAXY_COLORS[i]);
            colorLayerMaterials[i + PLANET_LAYERS_COUNT]->SetTexture("_SpriteTex", colorLayerSprite.get());
            colorLayerMaterials[i + PLANET_LAYERS_COUNT]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

            colorLayersTransform.Position.y += COLOR_LAYER_SIZE.y * 0.75f;
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

        colorBarManager->CreateAnimator(ColorBarType::Planet);
        colorBarManager->CreateAnimator(ColorBarType::Galaxy);
        colorBarManager->ForceSetColorsFromSeed();
    }
} // namespace Beer::System
