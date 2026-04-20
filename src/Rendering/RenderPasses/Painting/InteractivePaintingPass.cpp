#include "Rendering/RenderPasses/Painting/InteractivePaintingPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "InteractivePaintingPass.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "glm/fwd.hpp"
#include <memory>

namespace Beer::Rendering
{
    InteractivePaintingPass::InteractivePaintingPass(Material* displayMaterial)
        : IRenderPass("Interactive Painting Pass", RenderPassEvent::PRE_USER_INTERFACE), displayMaterial(displayMaterial)
    {
        paintingCompContext = std::make_shared<ComputeContext>("Painting/InteractivePainting");
        paintingCompContext->GetCompute()->PrintConfig();
        paintingKernel = paintingCompContext->GetCompute()->GetKernelIndex("PaintMouse");
        displayMaterial->GetShader()->PrintConfig();
    }

    void InteractivePaintingPass::OnRenderSetup(const RenderContext& context)
    {
        RenderTexture* paintTexture = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(INTERACTIVE_PAINT_NAME,
                                I_PAINT_RES_X,
                                I_PAINT_RES_Y,
                                static_cast<VkFormat>(vk::Format::eR16G16B16A16Unorm),
                                TextureAccess::ReadWrite)
                .AllocPointer);

        SetPaintingParams(paintTexture);
        displayMaterial->SetTexture("_SpriteTex", paintTexture);
        displayMaterial->SetVector("_Scale", glm::vec4(1));
        displayMaterial->SetColor("_TintColor", glm::vec4(1));
    }

    void InteractivePaintingPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetCustom(I_PAINT_RES_X, 8, I_PAINT_RES_Y, 8);
        commandBuffer->BindComputeKernel(paintingCompContext->GetCompute()->GetKernel(paintingKernel));
        commandBuffer->BindComputeContext(paintingCompContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList InteractivePaintingPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(INTERACTIVE_PAINT_NAME,
            ResourceAction::ComputeReadWrite));
        return dependencies;
    }

    void InteractivePaintingPass::SetPaintingParams(RenderTexture* paintTexture)
    {
        if (getMouseInput == nullptr || getCanvasTransform == nullptr)
        {
            return;
        }

        System::MouseInput input = getMouseInput();
        System::UITransform* canvasTransform = getCanvasTransform();

        paintingCompContext->SetTexture("_PaintTex", paintTexture);
        paintingCompContext->SetVector("_PaintResolution",
            glm::vec4(static_cast<float>(I_PAINT_RES_X), static_cast<float>(I_PAINT_RES_Y), 0, 0));

        paintingCompContext->SetVector("_RectTopRight", glm::vec4(canvasTransform->Rect.TopRight, 0, 0));
        paintingCompContext->SetVector("_RectBotRight", glm::vec4(canvasTransform->Rect.BotRight, 0, 0));
        paintingCompContext->SetVector("_RectTopLeft", glm::vec4(canvasTransform->Rect.TopLeft, 0, 0));
        paintingCompContext->SetVector("_RectBotLeft", glm::vec4(canvasTransform->Rect.BotLeft, 0, 0));

        paintingCompContext->SetVector("_MousePos", glm::vec4(input.PixelPos, 0, 0));
        paintingCompContext->SetInt("_MouseClick", input.LeftClickHold ? 1 : 0);

        paintingCompContext->Update();
    }
} // namespace Beer::Rendering
