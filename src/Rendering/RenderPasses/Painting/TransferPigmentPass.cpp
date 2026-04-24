#include "Rendering/RenderPasses/Painting/TransferPigmentPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    TransferPigmentPass::TransferPigmentPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("TransferPigmentPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 5)
    {
    }

    void TransferPigmentPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateCanvas(context);
        simulationBuffers->ReallocateSuspended(context);
        simulationBuffers->ReallocateDeposited(context);
        simulationBuffers->SimulationContext->SetTexture("_CanvasTarget", simulationBuffers->CanvasBuffer);
        simulationBuffers->SimulationContext->SetTexture("_SuspendedPigmentTarget", simulationBuffers->GetSuspendedPong(false));
        simulationBuffers->SimulationContext->SetTexture("_DepositedPigment", simulationBuffers->DepositedPigment);
        simulationBuffers->SimulationContext->SetFloat("_TransferSpeed", 10.0f);
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));
        simulationBuffers->SimulationContext->Update();

        simulationBuffers->debugMaterial->SetTexture("_DebugArrayTex", simulationBuffers->DepositedPigment);
    }

    void TransferPigmentPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8, PIGMENT_LAYERS, 1);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(TRANSFER_PGMNT_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList TransferPigmentPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(CANVAS_BUFFER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_A,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_B,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(DEPOSITED_PIGMENT,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }

} // namespace Beer::Rendering
