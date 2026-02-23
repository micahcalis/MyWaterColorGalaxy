#pragma once

#include <Vendor/stb/stb_image.h>
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    struct ImageAsset
    {
        int Width;
        int Height;
        int Channels;
        stbi_uc* Pixels;
        vk::DeviceSize Size;
    };
} // namespace Beer::Core
