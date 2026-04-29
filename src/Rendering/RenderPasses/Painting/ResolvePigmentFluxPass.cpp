#include "Rendering/RenderPasses/Painting/ResolvePigmentFluxPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    ResolvePigmentFluxPass::ResolvePigmentFluxPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("ResolvePigmentFluxPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 3)
    {
    }

    void ResolvePigmentFluxPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateWater(context);
        simulationBuffers->ReallocateFlux(context);
        simulationBuffers->ReallocateSuspended(context);
        simulationBuffers->SimulationContext->SetTexture("_ShallowWater", simulationBuffers->ShallowWater);
        simulationBuffers->SimulationContext->SetTexture("_FluxBuffer", simulationBuffers->FluxBuffer);
        simulationBuffers->SimulationContext->SetTexture("_SuspendedPigmentSource", simulationBuffers->GetSuspendedPong(true));
        simulationBuffers->SimulationContext->SetTexture("_SuspendedPigmentTarget", simulationBuffers->GetSuspendedPong(false));
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));
        simulationBuffers->SimulationContext->Update();
    }

    void ResolvePigmentFluxPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8, PIGMENT_LAYERS, 1);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(PIGMENT_FLUX_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList ResolvePigmentFluxPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(SHALLOW_WATER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(FLUX_BUFFER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_A,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_B,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }

} // namespace Beer::Rendering
