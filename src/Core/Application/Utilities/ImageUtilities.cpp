#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "Core/Application/Utilities/RendererUtilities.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    vk::raii::ImageView ImageUtilities::CreateImageView(vk::raii::Image& image, vk::Format format, vk::ImageAspectFlagBits aspectFlags, const Device& device)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = format;
        viewInfo.subresourceRange = vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1);

        return vk::raii::ImageView(device.GetLogicalDevice(), viewInfo);
    }

    void ImageUtilities::CreateImage(uint32_t width,
        uint32_t height,
        vk::Format format,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        vk::raii::Image& image,
        vk::raii::DeviceMemory& imageMemory,
        const Device& device)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.format = format;
        imageInfo.extent = vk::Extent3D{width, height, 1};
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = vk::SampleCountFlagBits::e1;
        imageInfo.tiling = tiling;
        imageInfo.usage = usage;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;

        image = vk::raii::Image(device.GetLogicalDevice(), imageInfo);

        vk::MemoryRequirements memRequirements = image.getMemoryRequirements();

        vk::MemoryAllocateInfo allocateInfo{};
        allocateInfo.allocationSize = memRequirements.size;
        allocateInfo.memoryTypeIndex = RendererUtilities::FindMemoryType(memRequirements.memoryTypeBits, properties, device.GetPhysicalDevice());

        imageMemory = vk::raii::DeviceMemory(device.GetLogicalDevice(), allocateInfo);
        image.bindMemory(imageMemory, 0);
    }

    vk::Format ImageUtilities::FindSupportedFormat(const std::vector<vk::Format> candidates,
        vk::ImageTiling tiling,
        vk::FormatFeatureFlags features,
        const Device& device)
    {
        for (const auto format : candidates)
        {
            vk::FormatProperties properties = device.GetPhysicalDevice().getFormatProperties(format);

            if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
                return format;

            if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features))
                return format;
        }

        throw std::runtime_error("failed to find supported format");
    }

    vk::Format ImageUtilities::FindDepthFormat(const Device& device)
    {
        return FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment,
            device);
    }

    bool ImageUtilities::HasStencilComponent(vk::Format format)
    {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }
} // namespace Beer::Core
