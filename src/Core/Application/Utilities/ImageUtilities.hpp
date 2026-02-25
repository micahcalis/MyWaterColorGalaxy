#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Renderer/Device.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    class ImageUtilities
    {
    public:
        static void CreateImage(uint32_t width,
            uint32_t height,
            vk::Format format,
            vk::ImageTiling tiling,
            vk::ImageUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            vk::raii::Image& image,
            vk::raii::DeviceMemory& imageMemory,
            const Device& device);

        static vk::raii::ImageView CreateImageView(vk::raii::Image& image,
            vk::Format format,
            vk::ImageAspectFlagBits aspectFlags,
            const Device& device);

        static vk::ImageView CreateImageView(vk::Image image,
            vk::Format format,
            vk::ImageAspectFlagBits aspectFlags,
            const Device& device);

        static vk::Format FindSupportedFormat(const std::vector<vk::Format> candidates,
            vk::ImageTiling tiling,
            vk::FormatFeatureFlags features,
            const Device& device);

        static vk::Format FindDepthFormat(const Device& device);

        static bool HasStencilComponent(vk::Format format);
    };
} // namespace Beer::Core
