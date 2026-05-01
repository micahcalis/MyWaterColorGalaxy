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
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.4f, 0.8f);
    static const uint32_t COLOR_LAYERS_COUNT = 4;
    static const glm::vec2 COLOR_LAYER_SIZE = glm::vec2(0.3f, 0.1f);

    static const std::array<glm::vec4, COLOR_LAYERS_COUNT> LAYER_COLORS = {
        glm::vec4(0.8f, 0.2f, 0.1f, 1),
        glm::vec4(0.6f, 0.1f, 0.6f, 1),
        glm::vec4(0.2f, 0.7f, 0.7f, 1),
        glm::vec4(0.3f, 0.4f, 0.8f, 1)};

    ColorBarEntity::ColorBarEntity(Function<MouseInput> getMouseInput,
        Function<void> openColorPicker,
        BeerEvent<void(glm::vec4)>* onColorPicked,
        BeerEvent<void()>* onColorPickerClosed)
        : getMouseInput(getMouseInput), openColorPicker(openColorPicker), onColorPicked(onColorPicked), onColorPickerClosed(onColorPickerClosed), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
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
        colorLayersTransform.Pivot = AnchorMode::BottomMiddle;

        colorLayers.reserve(COLOR_LAYERS_COUNT);
        colorLayerMaterials.reserve(COLOR_LAYERS_COUNT);

        for (int i = 0; i < COLOR_LAYERS_COUNT; i++)
        {
            colorLayerMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            colorLayerMaterials[i]->SetColor("_TintColor", LAYER_COLORS[i]);
            colorLayerMaterials[i]->SetTexture("_SpriteTex", colorLayerSprite.get());
            colorLayerMaterials[i]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

            colorLayersTransform.Position.y += COLOR_LAYER_SIZE.y * 0.75f;
            colorLayers.emplace_back(std::make_unique<UISubEntity>(colorLayersTransform));
            rootTransform.BindChild(colorLayers[i]->GetTransform());
        }

        for (int i = static_cast<int>(colorLayers.size()) - 1; i >= 0; i--)
        {
            ColorBarLevel level = static_cast<ColorBarLevel>(i);
            ColorBarManager* colorBarManager = GetColorBarManager();

            colorBarManager->CreateColorBarController(level,
                colorLayers[i]->GetTransform(),
                colorLayerMaterials[i].get(),
                LAYER_COLORS[i]);
        }
    }
} // namespace Beer::System
