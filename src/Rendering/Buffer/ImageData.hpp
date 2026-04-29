#pragma once

#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct ImageData
    {
    public:
        vk::Extent3D Extent;
        VkFormat Format;
        vk::ImageAspectFlags AspectMask = vk::ImageAspectFlagBits::eColor;
        uint32_t BaseMipLevel = 0;
        uint32_t MipLevels = VK_REMAINING_MIP_LEVELS;
        uint32_t BaseArrayLayer = 0;
        uint32_t ArrayLayers = VK_REMAINING_ARRAY_LAYERS;

    public:
        size_t GetSize() const
        {
            size_t pixelSize = Core::ImageUtilities::GetFormatData(vk::Format(Format)).PixelSize;
            return Extent.width * Extent.height * Extent.depth * pixelSize;
        }
    };
} // namespace Beer::Rendering
