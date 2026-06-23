#include "System/PaintTool/ToolBar/ToolBarEntity.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyComponent.hpp"
#include "ToolBarEntity.hpp"
#include "ToolBarManager.hpp"
#include <print>

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.6f, 1.5f);
    static const glm::vec2 BUTTON_SIZE = glm::vec2(0.25f, 0.25f);
    static const float BUTTON_PADDING = 0.02f;

    static const uint32_t BRUSH_COUNT = 5;
    static const std::array<std::string, BRUSH_COUNT> BRUSH_TEXTURE_PATHS = {
        "UI/ToolBar/Tex_PlanetButton",
        "UI/ToolBar/Tex_AsteroidsButton",
        "UI/ToolBar/Tex_SpacegooButton",
        "UI/ToolBar/Tex_BlackholeButton",
        "UI/ToolBar/Tex_StardustButton"};

    static const std::array<GalaxyBrushType, BRUSH_COUNT> BRUSH_TYPES = {
        GalaxyBrushType::Planet,
        GalaxyBrushType::Asteroids,
        GalaxyBrushType::SpaceGoo,
        GalaxyBrushType::BlackHole,
        GalaxyBrushType::StarDust};

    static const std::array<std::string, 2> HISTORY_BUTTON_TEXTURE_PATHS = {
        "UI/ToolBar/Tex_UndoButton",
        "UI/ToolBar/Tex_RedoButton"};

    static const glm::vec2 COLOR_DISPLAY_DIM = glm::vec2(0.15f, 0.45f);

    ToolBarEntity::ToolBarEntity(Function<void, GalaxyBrushType> setBrushType)
        : setBrushType(setBrushType)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
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
            GalaxyBrushType type = BRUSH_TYPES[i];

            toolBarManager->CreateGalaxyBrushController(brushes[i]->GetTransform(),
                brushMaterials[i].get(),
                type);
        }

        toolBarManager->SetCurrentBrush(GalaxyBrushType::Planet);
        MarkDirty();
    }

    void ToolBarEntity::InitializeTools(const std::vector<GalaxyBrushType>& toolTypes,
        std::vector<std::unique_ptr<UISubEntity>>& toolEntities,
        std::vector<std::shared_ptr<Rendering::Material>>& toolMaterials)
    {
        for (int i = 0; i < toolTypes.size(); i++)
        {
            GetToolBarManager()->CreateGalaxyBrushController(toolEntities[i]->GetTransform(),
                toolMaterials[i].get(),
                toolTypes[i]);
        }
    }

    void ToolBarEntity::InitializeHistoryButtons(Function<uint32_t, const GalaxyComponentData&> addComponent,
        Function<void, uint32_t> eraseComponent,
        std::vector<std::unique_ptr<UISubEntity>>& historyButtons,
        std::vector<std::shared_ptr<Rendering::Material>>& historyMaterials)
    {
        ToolBarManager* toolBarManager = GetToolBarManager();

        toolBarManager->CreateHistoryButtons(historyButtons[0]->GetTransform(),
            historyMaterials[0].get(),
            historyButtons[1]->GetTransform(),
            historyMaterials[1].get(),
            addComponent,
            eraseComponent);
    }

    void ToolBarEntity::InitializeColorDisplay()
    {
        UITransform colorDisplayTransform{};
        colorDisplayTransform.Anchor = AnchorMode::MiddleRight;
        colorDisplayTransform.Pivot = AnchorMode::MiddleLeft;
        colorDisplayTransform.Scale = COLOR_DISPLAY_DIM;

        colorDisplaySubEntity = std::make_unique<ColorDisplaySubEntity>(colorDisplayTransform,
            &rootTransform);

        GetToolBarManager()->InitializeColorDisplay(colorDisplaySubEntity.get());
    }
} // namespace Beer::System
