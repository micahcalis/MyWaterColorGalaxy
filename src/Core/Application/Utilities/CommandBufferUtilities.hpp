#pragma once

#include "Core/Application/Renderer/FrameResource.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "Rendering/Buffer/Buffer.hpp"

namespace Beer::Core
{
    class CommandBufferUtilities
    {
    public:
        static void TransitionImageLayout(vk::CommandBuffer& commandBuffer,
            vk::Image image,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            vk::AccessFlags2 srcAccessMask,
            vk::AccessFlags2 dstAccessMask,
            vk::PipelineStageFlags2 srcStageMask,
            vk::PipelineStageFlags2 dstStageMask,
            vk::ImageAspectFlags imageAspectFlags);

        static void TransitionImageLayout(const vk::raii::Image& image,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            const FrameResource& frameResource,
            const Device& device);

        // TODO: don't hardcode vertices X), I second that one.
        static void DrawCall(vk::CommandBuffer commandBuffer,
            const vk::raii::Pipeline& pipeline,
            const VkBuffer& vertexBuffer);

        static void DrawIndexedCall(vk::CommandBuffer commandBuffer,
            const vk::raii::Pipeline& pipeline,
            const VkBuffer& vertexBuffer,
            const VkBuffer& indexBuffer,
            const uint32_t indexCount);

        static vk::raii::CommandBuffer BeginSingleTimeCommands(const FrameResource& frameResource,
            const Device& device);

        static void EndSingleTimeCommands(vk::raii::CommandBuffer& commandBuffer, const Device& device);

        static void CopyBufferToImage(const Rendering::Buffer& buffer,
            vk::raii::Image& image,
            uint32_t width,
            uint32_t height,
            const FrameResource& frameResource,
            const Device& device);
    };
} // namespace Beer::Core
