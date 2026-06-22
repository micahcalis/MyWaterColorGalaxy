#include "Rendering/RenderPasses/Watercolor/FilterPostProcessingPass.hpp"

namespace Beer::Rendering
{
    void FilterPostProcessingPass::OnRenderSetup(const RenderContext& context)
    {
    }

    void FilterPostProcessingPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(context.GetMainColorSource(),
            postProcessingMaterial.get(),
            ShaderPassType::BlitColor,
            context.Output);
    }

    PassDependencyList FilterPostProcessingPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependencies(RenderGlobalUtilities::GetMainColorBlitDependencies());

        return dependencies;
    }

} // namespace Beer::Rendering
