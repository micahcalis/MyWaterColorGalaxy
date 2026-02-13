#pragma once

#include "Core/Application/Utilities/RendererUtilities.hpp"
#include "RendererUtilities.hpp"
#include "vulkan/vulkan.hpp"
#include <iostream>
#include <print>

namespace Beer::Core
{
    bool RendererUtilities::AcquireNextImage(Swapchain& swapchain,
        const FrameResource& frameResource,
        uint32_t& imageIndex)
    {
        // try
        // {
        //     auto acquireResult = swapchain.AcquireNextImage(*frameResource.GetImageAvailableSemaphore());
        //     result = acquireResult.result;
        //     imageIndex = acquireResult.value;
        // } catch (const vk::OutOfDateKHRError& e)
        // {
        //     return false;
        // }
        auto [result, index] = swapchain.AcquireNextImage(*frameResource.GetImageAvailableSemaphore());
        imageIndex = index;
        if (result == vk::Result::eSuccess)
            return true;

        assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady && "Swapchain result timeout or not ready");

        std::println(std::cerr, "failed to acquire swap chain image!");

        return false;
    }

    vk::RenderingAttachmentInfo RendererUtilities::CreateColorAttachmentInfo(vk::ImageView imageView, vk::ClearValue clearColor)
    {
        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.imageView = imageView;
        colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.clearValue = clearColor;

        return colorAttachment;
    }

    vk::RenderingInfo RendererUtilities::CreateRenderingInfo(vk::Extent2D extent, const vk::RenderingAttachmentInfo& colorAttachment)
    {
        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea = vk::Rect2D({0, 0}, extent);
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        return renderingInfo;
    }

    vk::SubmitInfo RendererUtilities::CreateSubmitInfo(const FrameResource& frameResource,
        const vk::CommandBuffer& commandBuffer,
        vk::PipelineStageFlags* waitMask)
    {
        vk::SubmitInfo submitInfo{};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &*frameResource.GetImageAvailableSemaphore();
        submitInfo.pWaitDstStageMask = waitMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &*frameResource.GetRenderFinishedSemaphore();

        return submitInfo;
    }

    vk::PresentInfoKHR RendererUtilities::CreatePresentInfo(const FrameResource& frameResource,
        const Swapchain& swapchain,
        const uint32_t& imageIndex)

    {
        auto& raiiSwapchain = swapchain.GetRaiiSwapchain();

        vk::PresentInfoKHR presentInfoKHR{};
        presentInfoKHR.waitSemaphoreCount = 1;
        presentInfoKHR.pWaitSemaphores = &*frameResource.GetRenderFinishedSemaphore();
        presentInfoKHR.swapchainCount = 1;
        presentInfoKHR.pSwapchains = &*raiiSwapchain;
        presentInfoKHR.pImageIndices = &imageIndex;
        presentInfoKHR.pResults = nullptr;

        return presentInfoKHR;
    }

    /**
     * @brief vk::raii::Queue::presentKHR without exceptions
     * Source: https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
     */
    vk::Result RendererUtilities::Queue_PresentKHR_NoExcept(
        const vk::raii::Queue& queue,
        const vk::PresentInfoKHR& present_info)
    {
        return static_cast<vk::Result>(queue.getDispatcher()->vkQueuePresentKHR(
            static_cast<VkQueue>(*queue),
            reinterpret_cast<const VkPresentInfoKHR*>(&present_info)));
    } // namespace Beer::Core
} // namespace Beer::Core
