#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>
#include "Rendering/Compute/Threads.hpp"

namespace Beer::Rendering
{
    struct TextureMakeSettings
    {
    public:
        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t Depth = 1;
        VkFormat Format = VK_FORMAT_R8G8B8A8_UNORM;
        uint32_t LayerCount = 1;
        uint32_t KernelIndex = 0;
        uint32_t GroupSizeX = 8;
        uint32_t GroupSizeY = 8;
        uint32_t GroupSizeZ = 1;

    public:
        Threads GetThreads()
        {
            return Threads::GetCustom(Width, GroupSizeX, Height, GroupSizeY, Depth, GroupSizeZ);
        }
    };
} // namespace Beer::Rendering
