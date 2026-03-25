#include "Rendering/Buffer/Image.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "ImageData.hpp"
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

    Image Image::CreateImage2D(uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        vk::ImageAspectFlagBits aspectFlags,
        const Core::Device& device)
    {
        auto allocation = sharedAllocator->CreateImage(width,
            height,
            format,
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            usage,
            VMA_MEMORY_USAGE_AUTO);

        auto defaultView = Core::ImageUtilities::CreateImageView(allocation.Image,
            vk::Format(format),
            aspectFlags,
            device);

        ImageData data{};
        data.Extent = vk::Extent3D(width, height, 1);
        data.Format = format;
        data.AspectMask = aspectFlags;

        return {allocation, defaultView, data};
    }

    Image::Image(ImageAllocation allocation,
        VkImageView defaultView,
        ImageData data)
        : allocator(sharedAllocator), allocation(allocation), defaultView(defaultView), data(data)
    {
    }

    std::shared_ptr<Image> Image::GetAsset(const std::string& name)
    {
        return imageAssetManager->Get(name);
    }

    void Image::QueueTransitionLayout(const vk::Image image,
        const ImageData& imageData,
        const vk::raii::CommandBuffer& commandBuffer,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout)
    {
        vk::ImageMemoryBarrier barrier{};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;

        barrier.subresourceRange.aspectMask = imageData.AspectMask;
        barrier.subresourceRange.baseMipLevel = imageData.BaseMipLevel;
        barrier.subresourceRange.levelCount = imageData.MipLevels;
        barrier.subresourceRange.baseArrayLayer = imageData.BaseArrayLayer;
        barrier.subresourceRange.layerCount = imageData.ArrayLayers;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

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
        } else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
        } else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(sourceStage,
            destinationStage,
            {},
            nullptr,
            nullptr,
            barrier);
    }

    void Image::QueueImageClear(vk::raii::CommandBuffer& commandBuffer,
        vk::ClearColorValue clearColor)
    {
        ImageData data = GetData();

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal);

        vk::ImageSubresourceRange range{};
        range.aspectMask = data.AspectMask;
        range.baseMipLevel = data.BaseMipLevel;
        range.levelCount = data.MipLevels;
        range.baseArrayLayer = data.BaseArrayLayer;
        range.layerCount = data.ArrayLayers;

        commandBuffer.clearColorImage(GetHandle(),
            vk::ImageLayout::eTransferDstOptimal,
            clearColor,
            {range});

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    void Image::QueueDepthClear(vk::raii::CommandBuffer& commandBuffer,
        vk::ClearDepthStencilValue clearValue)
    {
        ImageData data = GetData();

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal);

        vk::ImageSubresourceRange range{};
        range.aspectMask = data.AspectMask;
        range.baseMipLevel = data.BaseMipLevel;
        range.levelCount = data.MipLevels;
        range.baseArrayLayer = data.BaseArrayLayer;
        range.layerCount = data.ArrayLayers;

        commandBuffer.clearDepthStencilImage(GetHandle(),
            vk::ImageLayout::eTransferDstOptimal,
            clearValue,
            {range});

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal);
    }
} // namespace Beer::Rendering
