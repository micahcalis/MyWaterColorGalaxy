#pragma once

#define VMA_VULKAN_VERSION 1004000
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Rendering
{
    struct BufferAllocation
    {
        // It's not strictly required to disable the copy operands, but you can uncomment to prevent copies from being made.
        // BufferAllocation& operator=(const BufferAllocation&) = delete;
        // BufferAllocation(const BufferAllocation&) = delete;

        // BufferAllocation& operator=(BufferAllocation&&) = default;
        // BufferAllocation(BufferAllocation&&) = default;

        VkBuffer Buffer;
        VmaAllocation Allocation;
        VmaAllocationInfo Info;
        VmaAllocator Allocator;
    };
} // namespace Beer::Rendering
