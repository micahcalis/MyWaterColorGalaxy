#include "System/PaintTool/ToolBar/ToolBarEntity.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyComponent.hpp"
#include "ToolBarManager.hpp"
#include <print>

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.6f, 1.5f);
    static const glm::vec2 BUTTON_SIZE = glm::vec2(0.25f, 0.25f);
    static const float BUTTON_PADDING = 0.02f;

    static const uint32_t BRUSH_COUNT = 8;
    static const std::array<std::string, BRUSH_COUNT> BRUSH_TEXTURE_PATHS = {
        "UI/ToolBar/Tex_PlanetButton",
        "UI/ToolBar/Tex_AsteroidsButton",
        "UI/ToolBar/Tex_SpacegooButton",
        "UI/ToolBar/Tex_BlackholeButton",
        "UI/ToolBar/Tex_StardustButton",
        "UI/ToolBar/Tex_EraserButton",
        "UI/ToolBar/Tex_HandButton",
        "UI/ToolBar/Tex_MagnifyingGlassButton"};

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
            GalaxyBrushType type = static_cast<GalaxyBrushType>(i);

            toolBarManager->CreateGalaxyBrushController(brushes[i]->GetTransform(),
                brushMaterials[i].get(),
                type);
        }

        toolBarManager->SetCurrentBrush(GalaxyBrushType::Planet);
        MarkDirty();
    }

    void ToolBarEntity::InitializeHistoryButtons(Function<uint32_t, const GalaxyComponentData&> addComponent,
        Function<void, uint32_t> eraseComponent)
    {
        UITransform historyTransform{};
        historyTransform.Anchor = AnchorMode::TopMiddle;
        historyTransform.Scale = BUTTON_SIZE;
        historyTransform.Position = glm::vec2(0, -0.1f) + glm::vec2(-BUTTON_SIZE.y - BUTTON_PADDING) * static_cast<float>(BRUSH_COUNT / 2);

        historyButtons.reserve(2);
        historyMaterials.reserve(2);
        historyTextures.reserve(2);

        historyTextures.emplace_back(std::make_shared<Rendering::Texture2D>(HISTORY_BUTTON_TEXTURE_PATHS[0]));
        historyMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
        historyMaterials[0]->SetColor("_TintColor", glm::vec4(1, 1, 1, 1));
        historyMaterials[0]->SetTexture("_SpriteTex", historyTextures[0].get());
        historyMaterials[0]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        historyTransform.Pivot = AnchorMode::TopRight;
        historyTransform.Position.x = -BUTTON_PADDING;

        historyButtons.emplace_back(std::make_unique<UISubEntity>(historyTransform));
        rootTransform.BindChild(historyButtons[0]->GetTransform());

        historyTextures.emplace_back(std::make_shared<Rendering::Texture2D>(HISTORY_BUTTON_TEXTURE_PATHS[1]));
        historyMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
        historyMaterials[1]->SetColor("_TintColor", glm::vec4(1, 1, 1, 1));
        historyMaterials[1]->SetTexture("_SpriteTex", historyTextures[1].get());
        historyMaterials[1]->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        historyTransform.Pivot = AnchorMode::TopLeft;
        historyTransform.Position.x = BUTTON_PADDING;

        historyButtons.emplace_back(std::make_unique<UISubEntity>(historyTransform));
        rootTransform.BindChild(historyButtons[1]->GetTransform());

        ToolBarManager* toolBarManager = GetToolBarManager();

        toolBarManager->CreateHistoryButtons(historyButtons[0]->GetTransform(),
            historyMaterials[0].get(),
            historyButtons[1]->GetTransform(),
            historyMaterials[1].get(),
            addComponent,
            eraseComponent);

        MarkDirty();
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

        std::println("intialize tool bar color display");
    }
} // namespace Beer::System
