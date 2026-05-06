#include "System/PaintTool/ToolBar/ToolBarEntity.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "ToolBarManager.hpp"

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.6f, 1.2f);
    static const glm::vec2 BUTTON_SIZE = glm::vec2(0.25f, 0.25f);
    static const float BUTTON_PADDING = 0.02f;

    static const uint32_t BRUSH_COUNT = 6;
    static const std::array<std::string, BRUSH_COUNT> BRUSH_TEXTURE_PATHS = {
        "UI/ToolBar/Tex_PlanetButton",
        "UI/ToolBar/Tex_AsteroidsButton",
        "UI/ToolBar/Tex_SpacegooButton",
        "UI/ToolBar/Tex_BlackholeButton",
        "UI/ToolBar/Tex_StardustButton",
        "UI/ToolBar/Tex_EraserButton"};

    ToolBarEntity::ToolBarEntity(Function<void, GalaxyBrushType> setBrushType)
        : setBrushType(setBrushType), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");

        backgroundMat = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMat->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        backgroundMat->SetTexture("_SpriteTex", squareTexture.get());
        backgroundMat->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        rootTransform.Anchor = AnchorMode::BottomLeft;
        rootTransform.Pivot = AnchorMode::BottomLeft;
        rootTransform.Position.x += 0.02f;
        rootTransform.Scale = PANEL_SIZE;
        MarkDirty();
    }

    void ToolBarEntity::InitializeBrushes()
    {
        UITransform brushesTransform{};
        brushesTransform.Anchor = AnchorMode::TopMiddle;
        brushesTransform.Scale = BUTTON_SIZE;
        brushesTransform.Position = glm::vec2(0, -0.1f);

        brushes.reserve(BRUSH_COUNT);
        brushMaterials.reserve(BRUSH_COUNT);
        brushTextures.reserve(BRUSH_COUNT);

        for (int i = 0; i < BRUSH_COUNT; i++)
        {
            brushTextures.emplace_back(std::make_shared<Rendering::Texture2D>(BRUSH_TEXTURE_PATHS[i]));
            brushMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            brushMaterials[i]->SetColor("_TintColor", glm::vec4(1, 1, 1, 1));
            brushMaterials[i]->SetTexture("_SpriteTex", brushTextures[i].get());
            brushMaterials[i]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

            bool isEven = i % 2 == 0;
            brushesTransform.Pivot = isEven ? AnchorMode::TopRight : AnchorMode::TopLeft;
            brushesTransform.Position.x = isEven ? -BUTTON_PADDING : BUTTON_PADDING;
            brushesTransform.Position.y += isEven && i != 0 ? -BUTTON_SIZE.y - BUTTON_PADDING : 0;

            brushes.emplace_back(std::make_unique<UISubEntity>(brushesTransform));
            rootTransform.BindChild(brushes[i]->GetTransform());
        }

        ToolBarManager* toolBarManager = GetToolBarManager();

        for (int i = 0; i < BRUSH_COUNT; i++)
        {
            GalaxyBrushType type = static_cast<GalaxyBrushType>(i);

            toolBarManager->CreateGalaxyBrushController(brushes[i]->GetTransform(),
                brushMaterials[i].get(),
                type);
        }

        toolBarManager->SetCurrentBrush(GalaxyBrushType::Planet);
    }

    void ToolBarEntity::InitializeHistoryButtons()
    {
    }
} // namespace Beer::System
