#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    void CommandBufferUtilities::TransitionImageLayout(vk::CommandBuffer& commandBuffer,
        vk::Image image,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask,
        vk::AccessFlags2 dstAccessMask,
        vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask)
    {
        vk::ImageMemoryBarrier2 barrier{};
        barrier.srcStageMask = srcStageMask;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstStageMask = dstStageMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange = {
            vk::ImageAspectFlagBits::eColor,
            0,
            1,
            0,
            1};

        vk::DependencyInfo dependencyInfo{};
        dependencyInfo.dependencyFlags = {};
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &barrier;

        commandBuffer.pipelineBarrier2(dependencyInfo);
    }

    void CommandBufferUtilities::DrawCall(vk::CommandBuffer commandBuffer,
        const vk::raii::Pipeline& pipeline,
        const VkBuffer& vertexBuffer)
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        commandBuffer.bindVertexBuffers(0, vk::Buffer(vertexBuffer), {0});
        commandBuffer.draw(3, 1, 0, 0);
    }

    void CommandBufferUtilities::DrawIndexedCall(vk::CommandBuffer commandBuffer,
        const vk::raii::Pipeline& pipeline,
        const VkBuffer& vertexBuffer,
        const VkBuffer& indexBuffer,
        const uint32_t indexCount)
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        commandBuffer.bindVertexBuffers(0, vk::Buffer(vertexBuffer), {0});
        commandBuffer.bindIndexBuffer(vk::Buffer(indexBuffer), 0, vk::IndexType::eUint16);
        commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
    }
} // namespace Beer::Core
