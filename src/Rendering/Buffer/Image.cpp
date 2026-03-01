#include "Rendering/Buffer/Image.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "vulkan/vulkan.hpp"
#include "Core/Application/Managers/ImageAssetManager.hpp"

namespace Beer::Rendering
{
    Image::~Image()
    {
        if (allocator)
        {
            allocator->DestroyImage(allocation);
            allocator->DestroyImageView(defaultView);
        }
    }

    Image Image::CreateImage2D(std::shared_ptr<BufferAllocator> allocator,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        vk::ImageAspectFlagBits aspectFlags,
        const Core::Device& device)
    {
        auto allocation = allocator->CreateImage(width,
            height,
            format,
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            usage,
            VMA_MEMORY_USAGE_AUTO);

        auto defaultView = Core::ImageUtilities::CreateImageView(allocation.Image,
            vk::Format(format),
            aspectFlags,
            device);

        return {std::move(allocator),
            allocation,
            defaultView,
            vk::Extent3D(width, height, 1),
            format};
    }

    Image::Image(std::shared_ptr<BufferAllocator> allocator,
        ImageAllocation allocation,
        VkImageView defaultView,
        vk::Extent3D extent,
        VkFormat format)
        : allocator(allocator), allocation(allocation), defaultView(defaultView), extent(extent), format(format)
    {
    }

    std::shared_ptr<Image> Image::GetAsset(const std::string& name)
    {
        return imageAssetManager->Get(name);
    }

    void Image::QueueTransitionLayout(const vk::Image image,
        const vk::raii::CommandBuffer& commandBuffer,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout)
    {
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
    }
} // namespace Beer::Rendering
