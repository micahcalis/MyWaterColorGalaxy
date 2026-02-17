#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Vertex.hpp"

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

        static void CreateBuffer(vk::DeviceSize size,
            vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            const Device& device,
            vk::raii::Buffer& buffer,
            vk::raii::DeviceMemory& bufferMemory);

        static uint32_t FindMemoryType(uint32_t typeFilter,
            vk::MemoryPropertyFlags properties,
            const vk::raii::PhysicalDevice& physicalDevice);

        static void MapVertices(vk::raii::Buffer& vertexBuffer,
            vk::raii::DeviceMemory& vertexBufferMemory,
            const std::vector<Rendering::Vertex>& vertices,
            size_t size);

        static void CopyBuffer(vk::raii::Buffer& srcBuffer,
            vk::raii::Buffer& dstBuffer,
            vk::DeviceSize size,
            const Device& device,
            const FrameResource& frameResource);

        static void CreateImage(uint32_t width,
            uint32_t height,
            vk::Format format,
            vk::ImageTiling tiling,
            vk::ImageUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            vk::raii::Image& image,
            vk::raii::DeviceMemory& imageMemory,
            const Device& device);
    };
} // namespace Beer::Core
