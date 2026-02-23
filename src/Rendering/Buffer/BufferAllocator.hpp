#pragma once

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
        const Core::Device& device; // A shared_ptr would likely be preferrable.

    public:
        BufferAllocator(const Core::Device& device, const vk::raii::Instance& instance);
        ~BufferAllocator();

        [[nodiscard]] VmaAllocator GetAllocator() const { return vmaAllocator; }

        /// When returning the BufferAllocation a "copy" is made, so marking the BufferAllocation as const doesn't do anything.
        [[nodiscard]] Rendering::BufferAllocation CreateBuffer(VkDeviceSize size,
            VkBufferUsageFlags usage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags flags) const;

        [[nodiscard]] Rendering::BufferAllocation CreateStagingBuffer(vk::DeviceSize size) const;
        void DestroyBuffer(Rendering::BufferAllocation& buffer);
        void DestroyImage(Rendering::BufferAllocation& image);
    };
} // namespace Beer::Rendering
