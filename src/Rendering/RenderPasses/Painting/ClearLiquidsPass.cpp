#include "Rendering/RenderPasses/Painting/ClearLiquidsPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    ClearLiquidsPass::ClearLiquidsPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("ClearLiquidsPass", RenderPassEvent::WATER_COL_SIM)
    {
    }

    void ClearLiquidsPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateWater(context);
        simulationBuffers->ReallocateSuspended(context);
        simulationBuffers->ReallocateDeposited(context);
        simulationBuffers->ReallocateFlux(context);

        simulationBuffers->SimulationContext->SetTexture("_ShallowWater", simulationBuffers->ShallowWater);
        simulationBuffers->SimulationContext->SetTexture("_SuspendedPigmentSource", simulationBuffers->GetSuspendedPong(true));
        simulationBuffers->SimulationContext->SetTexture("_SuspendedPigmentTarget", simulationBuffers->GetSuspendedPong(false));
        simulationBuffers->SimulationContext->SetTexture("_FluxBuffer", simulationBuffers->FluxBuffer);
        simulationBuffers->SimulationContext->Update();
    }

    void ClearLiquidsPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8, PIGMENT_LAYERS, 1);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(CLEAR_LQDS_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList ClearLiquidsPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(SHALLOW_WATER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_A,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_B,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(FLUX_BUFFER,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }

} // namespace Beer::Rendering
