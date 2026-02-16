#pragma once

#define VMA_VULKAN_VERSION 1004000
#include <vulkan/vk_mem_alloc.h>
#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Buffer/BufferAllocation.hpp"

namespace Beer::Core
{
    class BufferAllocator
    {
    private:
        VmaAllocator vmaAllocator;
        const Device& device;

    public:
        BufferAllocator(const Device& device, const vk::raii::Instance& instance);
        ~BufferAllocator();
        const Rendering::BufferAllocation CreateBuffer(VkDeviceSize size,
            VkBufferUsageFlags usage,
            VmaMemoryUsage memoryUsage) const;
        const Rendering::BufferAllocation CreateStagingBuffer(vk::DeviceSize size) const;
        void DestroyBuffer(const Rendering::BufferAllocation& buffer);
        void DestroyImage(const Rendering::BufferAllocation& image);
    };
} // namespace Beer::Core
