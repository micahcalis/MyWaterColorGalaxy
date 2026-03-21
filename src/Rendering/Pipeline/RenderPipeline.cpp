#include "Rendering/Pipeline/RenderPipeline.hpp"
#include "CommandBuffer/RenderContext.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Light/ILight.hpp"
#include <memory>
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"

namespace Beer::Rendering
{
    RenderPipeline::RenderPipeline(const Core::Device* device,
        Core::UploadManager* uploadManager,
        System::RenderRegister* renderRegister)
    {
        frameBlackbox = std::make_unique<FrameBlackbox>(device, uploadManager);

        drawOpaquePass = std::make_unique<DrawOpaquePass>();

        colorTarget = frameBlackbox->CreateRenderTexture2D(std::string(Rendering::MAIN_COLOR),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::ColorFormat());

        depthTarget = frameBlackbox->CreateRenderTexture2D(std::string(Rendering::MAIN_DEPTH),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::DepthFormat());

        this->renderRegister = renderRegister;
    }

    void RenderPipeline::InitializeFrame()
    {
        frameBuilder = FrameBuilder(GetRenderPasses());
        frameGraph = frameBuilder.BuildGraph();

        colorTarget = frameBlackbox->ReallocateIfNeeded(std::string(Rendering::MAIN_COLOR),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::ColorFormat());

        depthTarget = frameBlackbox->ReallocateIfNeeded(std::string(Rendering::MAIN_DEPTH),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::DepthFormat());

        RenderContext context = GetRenderContext();
        frameGraph.OnRenderSetup(context);
        frameGraph.PrepareBarriers(context);
    }

    void RenderPipeline::ExecuteFrame(CommandBuffer* commandBuffer)
    {
        RenderContext context = GetRenderContext();
        frameGraph.Execute(commandBuffer, context);
    }

    void RenderPipeline::FinalBlit(CommandBuffer* commandBuffer, vk::Image swapchainImage, vk::Extent2D swapchainExtent)
    {
        vk::CommandBuffer cmd = commandBuffer->GetVk();

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

    std::vector<IRenderPass*> RenderPipeline::GetRenderPasses()
    {
        return {drawOpaquePass.get()};
    }

    RenderContext RenderPipeline::GetRenderContext()
    {
        RenderContext context{};
        context.BlackBox = frameBlackbox.get();
        context.Camera = System::Camera::Main();
        context.MainLight = System::ILight::Main();
        context.MainColorTarget = colorTarget;
        context.MainDepthTarget = depthTarget;
        context.Register = renderRegister;
        return context;
    }
} // namespace Beer::Rendering
