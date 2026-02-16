#pragma once

#define VMA_VULKAN_VERSION 1004000
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Rendering
{
    struct BufferAllocation
    {
        VkBuffer Buffer;
        VmaAllocation Allocation;
        VmaAllocationInfo Info;
    };
} // namespace Beer::Rendering
