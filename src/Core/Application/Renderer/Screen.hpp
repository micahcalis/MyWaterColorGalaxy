#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>

namespace Beer::Core
{
    class Screen
    {
        friend class Renderer;

    private:
        uint32_t width{};
        uint32_t height{};
        VkFormat colorFormat{};
        VkFormat depthFormat{};

    public:
        Screen(uint32_t width, uint32_t height, VkFormat colorFormat, VkFormat depthFormat)
            : width(width), height(height), colorFormat(colorFormat), depthFormat(depthFormat)
        {
        }

    private:
        static Screen instance;

        static void SetScreen(const Screen screen)
        {
            instance = screen;
        }

    public:
        static uint32_t Width() { return instance.width; }
        static uint32_t Height() { return instance.height; }
        static VkFormat ColorFormat() { return instance.colorFormat; }
        static VkFormat DepthFormat() { return instance.depthFormat; }
    };
} // namespace Beer::Core
