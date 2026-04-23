#include "Rendering/RenderPasses/Painting/ResolveFluidPass.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"

namespace Beer::Rendering
{
    ResolveFluidPass::ResolveFluidPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("ResolveFluidPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 3)
    {
    }

    void ResolveFluidPass::OnRenderSetup(const RenderContext& context)
    {
    }

    void ResolveFluidPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
    }

    PassDependencyList ResolveFluidPass::GetDependencies() const
    {
    }
} // namespace Beer::Rendering
