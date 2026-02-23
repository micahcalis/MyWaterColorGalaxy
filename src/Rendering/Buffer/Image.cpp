#include "Rendering/Buffer/Image.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "vulkan/vulkan.hpp"

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
        const Core::Device& device)
    {
        auto allocation = allocator->CreateImage(width,
            height,
            format,
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
            VMA_MEMORY_USAGE_AUTO);

        auto defaultView = Core::ImageUtilities::CreateImageView(allocation.Image,
            vk::Format(format),
            vk::ImageAspectFlagBits::eColor,
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
} // namespace Beer::Rendering
