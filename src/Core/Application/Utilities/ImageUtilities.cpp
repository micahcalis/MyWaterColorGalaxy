#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    vk::raii::ImageView ImageUtilities::CreateImageView(vk::raii::Image& image, vk::Format format, const Device& device)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = format;
        viewInfo.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

        return vk::raii::ImageView(device.GetLogicalDevice(), viewInfo);
    }
} // namespace Beer::Core
