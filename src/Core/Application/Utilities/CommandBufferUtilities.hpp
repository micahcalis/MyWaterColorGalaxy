#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

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
            vk::PipelineStageFlags2 dstStageMask);

        // TODO: don't hardcode vertices X)
        static void DrawCall(vk::CommandBuffer commandBuffer,
            const vk::raii::Pipeline& pipeline,
            const vk::raii::Buffer& vertexBuffer);
    };
} // namespace Beer::Core
