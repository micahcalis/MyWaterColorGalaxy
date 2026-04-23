#include "Rendering/RenderPasses/Painting/ResolveFluidFluxPass.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    ResolveFluidFluxPass::ResolveFluidFluxPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("ResolveFluidFluxPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 3)
    {
    }

    void ResolveFluidFluxPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateWater(context);
        simulationBuffers->ReallocateFlux(context);
        simulationBuffers->SimulationContext->SetTexture("_ShallowWater", simulationBuffers->ShallowWater);
        simulationBuffers->SimulationContext->SetTexture("_FluxBuffer", simulationBuffers->FluxBuffer);
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));
        simulationBuffers->SimulationContext->Update();
    }

    void ResolveFluidFluxPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(RESOLVE_FLUX_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList ResolveFluidFluxPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(SHALLOW_WATER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(FLUX_BUFFER,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }

} // namespace Beer::Rendering
