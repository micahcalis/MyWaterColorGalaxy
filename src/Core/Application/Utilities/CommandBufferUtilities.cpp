#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "Core/Application/Renderer/FrameResource.hpp"
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

    void CommandBufferUtilities::TransitionImageLayout(const vk::raii::Image& image,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        const FrameResource& frameResource,
        const Device& device)
    {
        vk::raii::CommandBuffer commandBuffer = BeginSingleTimeCommands(frameResource, device);

        vk::ImageMemoryBarrier barrier{};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;
        barrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(sourceStage,
            destinationStage,
            {},
            {},
            nullptr,
            barrier);

        EndSingleTimeCommands(commandBuffer, device);
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

    vk::raii::CommandBuffer CommandBufferUtilities::BeginSingleTimeCommands(const FrameResource& frameResource, const Device& device)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = frameResource.GetCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        auto cmdBuffers = device.GetLogicalDevice().allocateCommandBuffers(allocInfo);
        vk::raii::CommandBuffer commandBuffer = std::move(cmdBuffers.front());

        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        return commandBuffer;
    }

    void CommandBufferUtilities::EndSingleTimeCommands(vk::raii::CommandBuffer& commandBuffer, const Device& device)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*commandBuffer;

        device.GetGraphicsQueue().submit(submitInfo, nullptr);
        device.GetGraphicsQueue().waitIdle();
    }

    void CommandBufferUtilities::CopyBufferToImage(const Rendering::Buffer& buffer,
        vk::raii::Image& image,
        uint32_t width,
        uint32_t height,
        const FrameResource& frameResource,
        const Device& device)
    {
        vk::raii::CommandBuffer commandBuffer = BeginSingleTimeCommands(frameResource, device);

        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
        region.imageOffset = vk::Offset3D(0, 0, 0);
        region.imageExtent = vk::Extent3D(width, height, 1);

        commandBuffer.copyBufferToImage(buffer.GetHandle(), image, vk::ImageLayout::eTransferDstOptimal, {region});

        EndSingleTimeCommands(commandBuffer, device);
    }
} // namespace Beer::Core
