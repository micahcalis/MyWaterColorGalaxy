#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Renderer/FrameResource.hpp"

namespace Beer::Core
{
    class RendererUtilities
    {
    public:
        static bool AcquireNextImage(Swapchain& swapchain,
            const FrameResource& frameResource,
            uint32_t& imageIndex);

        static vk::RenderingAttachmentInfo CreateColorAttachmentInfo(vk::ImageView imageView,
            vk::ClearValue clearColor);

        static vk::RenderingInfo CreateRenderingInfo(vk::Extent2D extent,
            const vk::RenderingAttachmentInfo& colorAttachment);

        static vk::SubmitInfo CreateSubmitInfo(const FrameResource& frameResource,
            const vk::CommandBuffer& commandBuffer,
            vk::PipelineStageFlags* waitMask);

        static vk::PresentInfoKHR CreatePresentInfo(const FrameResource& frameResource,
            const Swapchain& swapchain,
            const uint32_t& imageIndex);

        static vk::Result Queue_PresentKHR_NoExcept(const vk::raii::Queue& queue,
            const vk::PresentInfoKHR& present_info);
    };
} // namespace Beer::Core
