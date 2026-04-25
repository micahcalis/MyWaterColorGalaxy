#include "Rendering/RenderPasses/Painting/RenderPigmentPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    RenderPigmentPass::RenderPigmentPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("RenderPigmentPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 6)
    {
    }

    void RenderPigmentPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateDeposited(context);
        simulationBuffers->ReallocateRender(context);
        simulationBuffers->SimulationContext->SetTexture("_DepositedPigment", simulationBuffers->DepositedPigment);
        simulationBuffers->SimulationContext->SetTexture("_PigmentRender", simulationBuffers->PigmentRender);
        simulationBuffers->SimulationContext->SetColor("_CanvasColor", glm::vec4(0.969f, 0.969f, 0.914, 1));
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));
        simulationBuffers->SimulationContext->Update();

        simulationBuffers->debugMaterial->SetTexture("_PaintTex", simulationBuffers->PigmentRender);
    }

    void RenderPigmentPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8, PIGMENT_LAYERS, 1);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(RENDER_PGMNT_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList RenderPigmentPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(DEPOSITED_PIGMENT,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(PIGMENT_RENDER,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }

} // namespace Beer::Rendering
