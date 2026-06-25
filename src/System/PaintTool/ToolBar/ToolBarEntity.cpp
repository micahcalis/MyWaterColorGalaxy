#include "System/PaintTool/ToolBar/ToolBarEntity.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"
#include "System/PaintTool/ColorDisplay/PlanetDisplayHandler.hpp"
#include "System/PaintTool/ColorDisplay/PlanetDisplaySubEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyComponent.hpp"
#include "ToolBarEntity.hpp"
#include "ToolBarManager.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>
#include <print>

namespace Beer::System
{
    static const glm::vec2 DISPLAY_SCALE = glm::vec2(0.9f, 0.9f * 1.4f);
    static const glm::vec2 DISPLAY_OFFSET = glm::vec2(0.18f, -0.05f);
    static const glm::vec2 BUTTON_SIZE = glm::vec2(0.15f, 0.15f);
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
    static const glm::vec2 COLOR_DISPLAY_OFFSET = glm::vec2(-0.16f, 0.285f);

    static const glm::vec2 HELP_BUTTON_SCALE = glm::vec2(0.1f);
    static const glm::vec2 HELP_BUTTON_OFFSET = glm::vec2(-0.1f, 0.02f);
    static const glm::vec2 HELP_POPUP_SCALE = glm::vec2(0.8f, 0.465f);
    static const glm::vec2 HELP_POPUP_OFFSET = glm::vec2(0.0f, 0.0f);
    static const std::string HELP_TEXT = "This is your Tool Bar! Select your Galaxy Object type, and use them to paint on the map, and change the size of the objects using the Slider. On the right side of the Galaxy Map are additional tools. You can select the Eraser to remove unwanted Objects. Made a mistake? Use the Undo and Redo buttons to correct them.";

    ToolBarEntity::ToolBarEntity(Function<void, GalaxyBrushType> setBrushType)
        : setBrushType(setBrushType)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_ToolFrame");
        buttonBackgroundTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_ToolBackground");

        backgroundMat = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMat->SetColor("_TintColor", glm::vec4(1));
        backgroundMat->SetTexture("_SpriteTex", backgroundTexture.get());
        backgroundMat->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        buttonBackgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        buttonBackgroundMaterial->SetColor("_TintColor", glm::vec4(1));
        buttonBackgroundMaterial->SetTexture("_SpriteTex", buttonBackgroundTexture.get());
        buttonBackgroundMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        rootTransform.Anchor = AnchorMode::MiddleLeft;
        rootTransform.Pivot = AnchorMode::MiddleLeft;
        rootTransform.Position.x += 0.02f;
        rootTransform.Scale = DISPLAY_SCALE;
        rootTransform.Position = DISPLAY_OFFSET;
        rootTransform.Depth = 0.2f;
        MarkDirty();
    }

    void ToolBarEntity::InitializeBrushes()
    {
        UITransform brushesTransform{};
        brushesTransform.Anchor = AnchorMode::TopLeft;
        brushesTransform.Pivot = AnchorMode::TopLeft;
        brushesTransform.Scale = BUTTON_SIZE;
        brushesTransform.Position = glm::vec2(0.05f, -0.05f);
        brushesTransform.Depth = 0.25f;

        brushes.reserve(BRUSH_COUNT);
        brushMaterials.reserve(BRUSH_COUNT);
        brushTextures.reserve(BRUSH_COUNT);

        for (int i = 0; i < BRUSH_COUNT; i++)
        {
            brushTextures.emplace_back(std::make_shared<Rendering::Texture2D>(BRUSH_TEXTURE_PATHS[i],
                Rendering::Sampler::Get(vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge)));

            brushMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            brushMaterials[i]->SetColor("_TintColor", glm::vec4(1, 1, 1, 1));
            brushMaterials[i]->SetTexture("_SpriteTex", brushTextures[i].get());
            brushMaterials[i]->SetVector("_Scale", glm::vec4(1.5f, 1.5f, 0, 0));
            brushMaterials[i]->SetVector("_Offset", glm::vec4(-0.3f, -0.3f, 0, 0));

            brushes.emplace_back(std::make_unique<UISubEntity>(brushesTransform));
            rootTransform.BindChild(brushes[i]->GetTransform());

            brushesTransform.Position.y += -BUTTON_SIZE.y - BUTTON_PADDING;
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
        colorDisplayTransform.Position = COLOR_DISPLAY_OFFSET;
        colorDisplayTransform.Depth = 0.25f;

        colorDisplaySubEntity = std::make_unique<ColorDisplaySubEntity>(colorDisplayTransform,
            &rootTransform);

        GetToolBarManager()->InitializeColorDisplay(colorDisplaySubEntity.get());
    }

    void ToolBarEntity::InitializePlanetDisplay()
    {
        planetDisplaySubEntity = std::make_unique<PlanetDisplaySubEntity>(&rootTransform);
        GetToolBarManager()->InitializePlanetDisplay(planetDisplaySubEntity.get());
    }

    void ToolBarEntity::InitializeHelpButton()
    {
        UITransform helpButtonTransform{};
        helpButtonTransform.Anchor = AnchorMode::TopRight;
        helpButtonTransform.Pivot = AnchorMode::TopLeft;
        helpButtonTransform.Scale = HELP_BUTTON_SCALE;
        helpButtonTransform.Position = HELP_BUTTON_OFFSET;
        helpButtonTransform.Depth = 0.5f;

        UITransform helpPopupTransform{};
        helpPopupTransform.Anchor = AnchorMode::Center;
        helpPopupTransform.Pivot = AnchorMode::Center;
        helpPopupTransform.Scale = HELP_POPUP_SCALE;
        helpPopupTransform.Position = HELP_POPUP_OFFSET;
        helpPopupTransform.Depth = 0.6f;

        helpButtonSubEntity = std::make_unique<HelpButtonSubEntity>(
            &rootTransform,
            helpButtonTransform,
            HELP_TEXT,
            helpPopupTransform);

        GetToolBarManager()->SetHelpToggle(helpButtonSubEntity.get(),
            [this]() -> void { MarkDirty(); });
    }
} // namespace Beer::System
