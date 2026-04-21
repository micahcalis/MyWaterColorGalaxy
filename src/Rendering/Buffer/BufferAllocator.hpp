#pragma once

#include "Rendering/Buffer/ImageAllocation.hpp"
#define VMA_VULKAN_VERSION 1004000
#include <vulkan/vk_mem_alloc.h>
#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Buffer/BufferAllocation.hpp"

namespace Beer::Rendering
{
    class BufferAllocator
    {
    private:
        VmaAllocator vmaAllocator;
        const Core::Device& device;

    public:
        BufferAllocator(const Core::Device& device, const vk::raii::Instance& instance);
        ~BufferAllocator();

        [[nodiscard]] VmaAllocator GetAllocator() const { return vmaAllocator; }

        /// When returning the BufferAllocation a "copy" is made, so marking the BufferAllocation as const doesn't do anything.
        [[nodiscard]] BufferAllocation CreateBuffer(VkDeviceSize size,
            VkBufferUsageFlags usage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags flags) const;

        [[nodiscard]] BufferAllocation CreateStagingBuffer(vk::DeviceSize size) const;

        [[nodiscard]] ImageAllocation CreateImage(uint32_t width,
            uint32_t height,
            VkFormat format,
            VkImageTiling tiling,
            VkImageUsageFlags usage,
            VmaMemoryUsage memoryUsage,
            uint32_t layerCount) const;

        void DestroyBuffer(BufferAllocation& buffer);
        void DestroyImage(ImageAllocation& image);
        void DestroyImageView(VkImageView imageView);
    };
} // namespace Beer::Rendering
