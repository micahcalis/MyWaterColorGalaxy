#include "Rendering/RenderPasses/Painting/InteractivePaintingPass.hpp"
#include "InteractivePaintingPass.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "glm/fwd.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    InteractivePaintingPass::InteractivePaintingPass(Material* displayMaterial)
        : IRenderPass("Interactive Painting Pass", RenderPassEvent::PRE_USER_INTERFACE), displayMaterial(displayMaterial)
    {
        paintingCompContext = std::make_shared<ComputeContext>("Painting/InteractivePainting");
        paintingKernel = paintingCompContext->GetCompute()->GetKernelIndex("PaintMouse");
        UpdateRandomColor();
    }

    void InteractivePaintingPass::OnRenderSetup(const RenderContext& context)
    {
        RenderTexture* paintTexture = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(INTERACTIVE_PAINT_NAME,
                                I_PAINT_RES_X,
                                I_PAINT_RES_Y,
                                static_cast<VkFormat>(vk::Format::eR16G16B16A16Unorm),
                                TextureAccess::ReadWrite,
                                vk::Filter::eLinear,
                                vk::SamplerAddressMode::eClampToEdge,
                                glm::vec4(1, 1, 1, 0))
                .AllocPointer);

        SetPaintingParams(paintTexture);

        System::ButtonInput debugKeyInput = getDebugKeyInput();
        if (debugKeyInput.ButtonStart)
        {
            UpdateRandomColor();
        }

        displayMaterial->SetTexture("_PaintTex", paintTexture);
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
        paintingCompContext->SetFloat("_PaintIntensity", 5.0f);

        paintingCompContext->Update();
    }

    void InteractivePaintingPass::UpdateRandomColor()
    {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        paintingCompContext->SetColor("_PaintColor", glm::vec4(r, g, b, 1));
        paintingCompContext->Update();
    }
} // namespace Beer::Rendering
