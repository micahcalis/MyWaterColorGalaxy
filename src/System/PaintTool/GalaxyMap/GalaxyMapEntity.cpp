#include "System/PaintTool/GalaxyMap/GalaxyMapEntity.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Context/ContextType.hpp"

namespace Beer::System
{
    static const float MAP_SCALE = 2.0f;

    GalaxyMapEntity::GalaxyMapEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        galaxyMaterial = std::make_shared<Rendering::Material>("UI/GalaxyMapSprite");

        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;
        rootTransform.Scale = glm::vec2(MAP_SCALE);

        Rendering::TextureMakeSettings makeSettings{};
        makeSettings.Width = 512;
        makeSettings.Height = 512;
        makeSettings.KernelIndex = 0;

        generatePerlinWorleyContext = std::make_shared<Rendering::ComputeContext>("Texture/ComputeNoise");
        generatePerlinWorleyContext->SetColor("_TestColor", glm::vec4(1, 0, 1, 1));
        generatePerlinWorleyContext->Update();

        perlinWorleyTexture = std::make_shared<Rendering::Texture2D>(Rendering::Texture2D::Make(
            makeSettings,
            generatePerlinWorleyContext.get()));

        galaxyMaterial->SetTexture("_NoiseTexture", perlinWorleyTexture.get());

        MarkDirty();
    }
} // namespace Beer::System
