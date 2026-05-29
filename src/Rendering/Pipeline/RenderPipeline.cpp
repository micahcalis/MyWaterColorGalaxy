#include "Rendering/Pipeline/RenderPipeline.hpp"
#include "CommandBuffer/RenderContext.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Buffer/SSBOType.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/Globals/ModelTransformData.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Context/IContext.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Light/ILight.hpp"
#include <memory>
#include <print>
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"

namespace Beer::Rendering
{
    RenderPipeline::RenderPipeline(const Core::Device* device,
        Core::UploadManager* uploadManager,
        System::RenderRegister* renderRegister)
    {
        frameBlackbox = std::make_unique<FrameBlackbox>(device, uploadManager);

        colorTargetA = frameBlackbox->CreateRenderTexture2D(std::string(MAIN_COLOR_A),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::ColorFormat());

        colorTargetB = frameBlackbox->CreateRenderTexture2D(std::string(MAIN_COLOR_B),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::ColorFormat());

        depthTarget = frameBlackbox->CreateRenderTexture2D(std::string(MAIN_DEPTH),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::DepthFormat());

        transformBuffer = frameBlackbox->CreatePhaseBuffer(std::string(TRANSFORM_BUFFER_NAME),
            PhaseBuffer::CalculateSize(TRANSFORM_BUFFER_COUNT, sizeof(ModelTransformData)),
            SSBOType::Persisent);

        Shader::Globals()->SetTransformBuffer(transformBuffer);

        this->renderRegister = renderRegister;
    }

    void RenderPipeline::InitializeFrame()
    {
        frameBuilder = FrameBuilder(GetSortedRenderPasses());
        frameGraph = frameBuilder.BuildGraph();

        colorTargetA = frameBlackbox->ReallocateIfNeeded(std::string(MAIN_COLOR_A),
                                        Core::Screen::Width(),
                                        Core::Screen::Height(),
                                        Core::Screen::ColorFormat())
                           .AllocPointer;

        colorTargetB = frameBlackbox->ReallocateIfNeeded(std::string(MAIN_COLOR_B),
                                        Core::Screen::Width(),
                                        Core::Screen::Height(),
                                        Core::Screen::ColorFormat())
                           .AllocPointer;

        depthTarget = frameBlackbox->ReallocateIfNeeded(std::string(MAIN_DEPTH),
                                       Core::Screen::Width(),
                                       Core::Screen::Height(),
                                       Core::Screen::DepthFormat())
                          .AllocPointer;

        auto [transformBuffer, transformAllocated] = frameBlackbox->ReallocateIfNeeded(std::string(TRANSFORM_BUFFER_NAME),
            PhaseBuffer::CalculateSize(TRANSFORM_BUFFER_COUNT, sizeof(ModelTransformData)),
            SSBOType::Persisent);

        if (transformAllocated)
        {
            Shader::Globals()->SetTransformBuffer(transformBuffer);
        }

        RenderContext context = GetRenderContext();
        frameGraph.OnRenderSetup(context);
        frameGraph.PrepareBarriers(context);
    }

    void RenderPipeline::ExecuteFrame(CommandBuffer* commandBuffer, bool& pongState)
    {
        RenderContext context = GetRenderContext();

        System::Function<void> bindGlobals = [commandBuffer, context]() {
            BindGlobals(commandBuffer, context);
        };

        frameGraph.Execute(commandBuffer, context, bindGlobals);
        pongState = context.GetPongState();
    }

    void RenderPipeline::FinalBlit(CommandBuffer* commandBuffer,
        vk::Image swapchainImage,
        vk::Extent2D swapchainExtent,
        bool pongState)
    {
        vk::CommandBuffer cmd = commandBuffer->GetVk();

        RenderTexture* colorTarget = pongState ? colorTargetB : colorTargetA;
        vk::Image colorImage = colorTarget->GetImage()->GetHandle();

        Core::CommandBufferUtilities::TransitionImageLayout(cmd,
            swapchainImage,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            {},
            vk::AccessFlagBits2::eTransferWrite,
            vk::PipelineStageFlagBits2::eTopOfPipe,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::ImageAspectFlagBits::eColor);

        Core::CommandBufferUtilities::TransitionImageLayout(cmd,
            colorImage,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::AccessFlagBits2::eTransferRead,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::ImageAspectFlagBits::eColor);

        vk::ImageBlit blit{};
        blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
        blit.srcOffsets[1] = vk::Offset3D{static_cast<int32_t>(colorTarget->Width()), static_cast<int32_t>(colorTarget->Height()), 1};
        blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.layerCount = 1;

        blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
        blit.dstOffsets[1] = vk::Offset3D{static_cast<int32_t>(swapchainExtent.width), static_cast<int32_t>(swapchainExtent.height), 1};
        blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.layerCount = 1;

        cmd.blitImage(colorImage,
            vk::ImageLayout::eTransferSrcOptimal,
            swapchainImage,
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &blit,
            vk::Filter::eLinear);

        Core::CommandBufferUtilities::TransitionImageLayout(cmd,
            swapchainImage,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eTransferWrite,
            {},
            vk::PipelineStageFlagBits2::eTransfer,
            vk::PipelineStageFlagBits2::eBottomOfPipe,
            vk::ImageAspectFlagBits::eColor);

        Core::CommandBufferUtilities::TransitionImageLayout(cmd,
            colorImage,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::AccessFlagBits2::eTransferRead,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::ImageAspectFlagBits::eColor);

        commandBuffer->End();
    }

    std::vector<IRenderPass*> RenderPipeline::GetSortedRenderPasses()
    {
        std::vector<IRenderPass*> passes = System::IContext::GetActivePasses();

        std::sort(passes.begin(), passes.end(), [](const IRenderPass* a, const IRenderPass* b) {
            if (a->GetEvent() == b->GetEvent())
            {
                return a < b;
            }
            return a->GetEvent() < b->GetEvent();
        });

        auto newEnd = std::unique(passes.begin(), passes.end());
        passes.erase(newEnd, passes.end());

        return passes;
    }

    RenderContext RenderPipeline::GetRenderContext()
    {
        RenderContext context{};
        context.BlackBox = frameBlackbox.get();
        context.Camera = System::Camera::Main();
        context.MainLight = System::ILight::Main();
        context.MainDepthTarget = depthTarget;
        context.TransformBuffer = transformBuffer;
        context.Register = renderRegister;
        context.SetMainColorTargets(colorTargetA, colorTargetB);
        return context;
    }

    void RenderPipeline::BindGlobals(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Rendering::Shader::Globals()->Bind(commandBuffer, vk::PipelineBindPoint::eGraphics);
        Rendering::Shader::Globals()->Bind(commandBuffer, vk::PipelineBindPoint::eCompute);
    }
} // namespace Beer::Rendering
