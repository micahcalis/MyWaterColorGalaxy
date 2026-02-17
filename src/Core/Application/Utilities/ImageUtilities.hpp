#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Renderer/Device.hpp"

namespace Beer::Core
{
    class ImageUtilities
    {
    public:
        static vk::raii::ImageView CreateImageView(vk::raii::Image& image,
            vk::Format format,
            const Device& device);
    };
} // namespace Beer::Core
