#include "System/PaintTool/ToolBar/MapBarEntity.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Sampler/Sampler.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::System
{
    static const glm::vec2 BACKGROUND_SIZE = glm::vec2(0.15f, 0.75f);
    static const glm::vec2 BACKGROUND_OFFSET = glm::vec2(0.23f, -0.39f);
    static const float BUTTON_SIZE = 0.15f;
    static const float BUTTON_PADDING = 0.02f;

    static const std::array<std::string, TOOL_COUNT> BRUSH_TEXTURE_PATHS = {
        "UI/ToolBar/Tex_EraserButton",
        "UI/ToolBar/Tex_HandButton",
        "UI/ToolBar/Tex_MagnifyingGlassButton"};

    static const std::array<std::string, 2> HISTORY_BUTTON_TEXTURE_PATHS = {
        "UI/ToolBar/Tex_UndoButton",
        "UI/ToolBar/Tex_RedoButton"};

    MapBarEntity::MapBarEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        rootTransform.Depth = 0.02f;

        UITransform backgroundTransform{};
        backgroundTransform.Anchor = AnchorMode::MiddleRight;
        backgroundTransform.Pivot = AnchorMode::MiddleLeft;
        backgroundTransform.Scale = BACKGROUND_SIZE;
        backgroundTransform.Position = BACKGROUND_OFFSET;

        backgroundSubEntity = std::make_unique<UISubEntity>(backgroundTransform);

        slotTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_ToolBackground2");
        slotMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        slotMaterial->SetTexture("_SpriteTex", slotTexture.get());
        slotMaterial->SetColor("_TintColor", glm::vec4(1));
        slotMaterial->SetVector("_Scale", glm::vec4(1));
    }

    void MapBarEntity::InitializeButtons(UITransform* parent,
        MapBarInitializationFunc initializeTools,
        MapBarInitializationFunc initializeHistoryButtons)
    {
        parent->BindChild(backgroundSubEntity->GetTransform());

        UITransform buttonTransform{};
        buttonTransform.Anchor = AnchorMode::TopMiddle;
        buttonTransform.Scale = glm::vec2(BUTTON_SIZE);
        buttonTransform.Position = glm::vec2(0, -BUTTON_PADDING);

        for (int i = 0; i < TOOL_COUNT; i++)
        {
            toolTextures.emplace_back(std::make_shared<Rendering::Texture2D>(BRUSH_TEXTURE_PATHS[i],
                Rendering::Sampler::Get(vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge)));

            toolMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
            toolMaterials[i]->SetColor("_TintColor", glm::vec4(1, 1, 1, 1));
            toolMaterials[i]->SetTexture("_SpriteTex", toolTextures[i].get());
            toolMaterials[i]->SetVector("_Scale", glm::vec4(1.45f, 1.45f, 0, 0));
            toolMaterials[i]->SetVector("_Offset", glm::vec4(-0.3f, -0.3f, 0, 0));

            toolEntities.emplace_back(std::make_unique<UISubEntity>(buttonTransform));
            backgroundSubEntity->GetTransform()->BindChild(toolEntities[i]->GetTransform());
            buttonTransform.Position.y += -BUTTON_SIZE - BUTTON_PADDING;
        }

        initializeTools(toolEntities, toolMaterials);

        historyButtons.reserve(2);
        historyMaterials.reserve(2);
        historyTextures.reserve(2);

        historyTextures.emplace_back(std::make_shared<Rendering::Texture2D>(HISTORY_BUTTON_TEXTURE_PATHS[0],
            Rendering::Sampler::Get(vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge)));

        historyMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
        historyMaterials[0]->SetColor("_TintColor", glm::vec4(1, 1, 1, 1));
        historyMaterials[0]->SetTexture("_SpriteTex", historyTextures[0].get());
        historyMaterials[0]->SetVector("_Scale", glm::vec4(1.45f, 1.45f, 0, 0));
        historyMaterials[0]->SetVector("_Offset", glm::vec4(-0.3f, -0.3f, 0, 0));

        historyButtons.emplace_back(std::make_unique<UISubEntity>(buttonTransform));
        backgroundSubEntity->GetTransform()->BindChild(historyButtons[0]->GetTransform());

        historyTextures.emplace_back(std::make_shared<Rendering::Texture2D>(HISTORY_BUTTON_TEXTURE_PATHS[1],
            Rendering::Sampler::Get(vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge)));

        historyMaterials.emplace_back(std::make_shared<Rendering::Material>("UI/SpriteDefault"));
        historyMaterials[1]->SetColor("_TintColor", glm::vec4(1, 1, 1, 1));
        historyMaterials[1]->SetTexture("_SpriteTex", historyTextures[1].get());
        historyMaterials[1]->SetVector("_Scale", glm::vec4(1.45f, 1.45f, 0, 0));
        historyMaterials[1]->SetVector("_Offset", glm::vec4(-0.3f, -0.3f, 0, 0));

        buttonTransform.Position.y += -BUTTON_SIZE - BUTTON_PADDING;

        historyButtons.emplace_back(std::make_unique<UISubEntity>(buttonTransform));
        backgroundSubEntity->GetTransform()->BindChild(historyButtons[1]->GetTransform());

        initializeHistoryButtons(historyButtons, historyMaterials);
    }
} // namespace Beer::System
