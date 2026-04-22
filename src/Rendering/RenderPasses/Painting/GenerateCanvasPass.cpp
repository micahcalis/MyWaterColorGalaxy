#include "Rendering/RenderPasses/Painting/GenerateCanvasPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    GenerateCanvasPass::GenerateCanvasPass(WaterColorSimBuffers* simulationBuffers)
        : simulationBuffers(simulationBuffers), IRenderPass("CanvasGeneration", RenderPassEvent::WATER_COL_SIM)
    {
        canvasBaseTexture = std::make_shared<Texture2D>(Image::GetAsset(CANVAS_BASE_NAME));
    }

    void GenerateCanvasPass::OnRenderSetup(const RenderContext& context)
    {
        simulationBuffers->ReallocateCanvas(context);
        simulationBuffers->SimulationContext->SetTexture("_CanvasSource", canvasBaseTexture.get());
        simulationBuffers->SimulationContext->SetTexture("_CanvasTarget", simulationBuffers->CanvasBuffer);
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));
        simulationBuffers->SimulationContext->Update();
    }

    void GenerateCanvasPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(CANVAS_GENERATION_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList GenerateCanvasPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(CANVAS_BUFFER,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }
} // namespace Beer::Rendering
