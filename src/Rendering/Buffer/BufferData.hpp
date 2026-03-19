#pragma once

#include <vulkan/vulkan.hpp>

namespace Beer::Rendering
{
    struct BufferData
    {
        VkDeviceSize Size = VK_WHOLE_SIZE;
        VkDeviceSize Offset = 0;
    };
} // namespace Beer::Rendering
