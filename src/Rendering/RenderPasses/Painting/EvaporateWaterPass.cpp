#include "Rendering./RenderPasses/Painting/EvaporateWaterPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    EvaporateWaterPass::EvaporateWaterPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("RenderPigmentPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 7)
    {
    }

    void EvaporateWaterPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateWater(context);
        simulationBuffers->SimulationContext->SetTexture("_ShallowWater", simulationBuffers->ShallowWater);
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));
        simulationBuffers->SimulationContext->SetFloat("_EvaporationRate", 0.001f);
        simulationBuffers->SimulationContext->Update();
    }

    void EvaporateWaterPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8, PIGMENT_LAYERS, 1);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(EVAPORATE_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList EvaporateWaterPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(SHALLOW_WATER,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }

} // namespace Beer::Rendering
