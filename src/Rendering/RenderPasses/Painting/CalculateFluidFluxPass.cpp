#include "Rendering/RenderPasses/Painting/CalculateFluidFluxPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    CalculateFluidFluxPass::CalculateFluidFluxPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("CalculateFluidFluxPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 2)
    {
    }

    void CalculateFluidFluxPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateWater(context);
        simulationBuffers->ReallocateFlux(context);
        simulationBuffers->ReallocateCanvas(context);
        simulationBuffers->SimulationContext->SetTexture("_ShallowWater", simulationBuffers->ShallowWater);
        simulationBuffers->SimulationContext->SetTexture("_FluxBuffer", simulationBuffers->FluxBuffer);
        simulationBuffers->SimulationContext->SetTexture("_CanvasTarget", simulationBuffers->CanvasBuffer);
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));
        simulationBuffers->SimulationContext->Update();
    }

    void CalculateFluidFluxPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(CALC_FLUX_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList CalculateFluidFluxPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(SHALLOW_WATER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(FLUX_BUFFER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(CANVAS_BUFFER,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }
} // namespace Beer::Rendering
