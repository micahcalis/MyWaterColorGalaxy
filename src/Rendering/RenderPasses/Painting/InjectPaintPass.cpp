#include "Rendering/RenderPasses/Painting/InjectPaintPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "System/Base/Input/ButtonInput.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorMixer/PigmentButton.hpp"
#include "System/Readback/IAsyncReadback.hpp"
#include "System/Readback/ImagePixelData.hpp"
#include "System/Readback/ImageReadback.hpp"
#include "System/Readback/ImageReadbackRequest.hpp"
#include <memory>
#include <print>

namespace Beer::Rendering
{
    InjectPaintPass::InjectPaintPass(WaterColorSimBuffers* simulationBuffers,
        System::Function<System::MouseInput> getMouseInput,
        System::Function<System::UITransform*> getCanvasTransform)
        : simulationBuffers(simulationBuffers), getMouseInput(getMouseInput), getCanvasTransform(getCanvasTransform), IRenderPass("InjectPaintPass", static_cast<uint32_t>(RenderPassEvent::WATER_COL_SIM) + 1)
    {
    }

    void InjectPaintPass::OnRenderSetup(const RenderContext& context)
    {
        System::PigmentType currentPigment = System::PigmentType::QuinacridoneRose;

        if (getCurrentPigment != nullptr)
        {
            currentPigment = getCurrentPigment();
        }

        simulationBuffers->ReallocateWater(context);
        simulationBuffers->ReallocateSuspended(context);
        simulationBuffers->SimulationContext->SetTexture("_ShallowWater", simulationBuffers->ShallowWater);
        simulationBuffers->SimulationContext->SetTexture("_SuspendedPigmentSource", simulationBuffers->GetSuspendedPong(true));
        simulationBuffers->SimulationContext->SetTexture("_SuspendedPigmentTarget", simulationBuffers->GetSuspendedPong(false));
        simulationBuffers->SimulationContext->SetFloat("_BrushRadius", 0.1f);
        simulationBuffers->SimulationContext->SetFloat("_BrushIntensity", 5.0f);
        simulationBuffers->SimulationContext->SetFloat("_BrushSmoothness", 0.5f);
        simulationBuffers->SimulationContext->SetInt("_BrushPigment", static_cast<int>(currentPigment));
        simulationBuffers->SimulationContext->SetVector("_PaintResolution", glm::vec4((float)SIMULATION_RES_X, (float)SIMULATION_RES_Y, 0, 0));

        SetMouseInput();
        SetCanvasRect();

        simulationBuffers->SimulationContext->Update();
    }

    void InjectPaintPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(SIMULATION_RES_X, 8, SIMULATION_RES_Y, 8);
        commandBuffer->BindComputeKernel(simulationBuffers->SimulationContext->GetCompute()->GetKernel(PAINT_INJECT_KERNEL));
        commandBuffer->BindComputeContext(simulationBuffers->SimulationContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList InjectPaintPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(SHALLOW_WATER,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_A,
            ResourceAction::ComputeReadWrite));

        dependencies.AddDependency(PassDependency(SUSPENDED_PIGMENT_B,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }

    void InjectPaintPass::SetMouseInput() const
    {
        System::MouseInput mouseInput = getMouseInput();
        simulationBuffers->SimulationContext->SetVector("_MousePos", glm::vec4(mouseInput.PixelPos.x, Core::Screen::Height() - mouseInput.PixelPos.y, 0, 0));
        simulationBuffers->SimulationContext->SetInt("_MouseClick", mouseInput.LeftClickHold ? 1 : 0);
    }

    void InjectPaintPass::SetCanvasRect() const
    {
        System::UITransform* canvasTransform = getCanvasTransform();
        simulationBuffers->SimulationContext->SetVector("_RectTopRight", glm::vec4(canvasTransform->Rect.TopRight, 0, 0));
        simulationBuffers->SimulationContext->SetVector("_RectBotRight", glm::vec4(canvasTransform->Rect.BotRight, 0, 0));
        simulationBuffers->SimulationContext->SetVector("_RectTopLeft", glm::vec4(canvasTransform->Rect.TopLeft, 0, 0));
        simulationBuffers->SimulationContext->SetVector("_RectBotLeft", glm::vec4(canvasTransform->Rect.BotLeft, 0, 0));
    }
} // namespace Beer::Rendering
