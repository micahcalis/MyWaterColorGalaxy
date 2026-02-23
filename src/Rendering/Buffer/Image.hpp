#pragma once

#include "ImageAllocation.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Buffer/ImageAllocation.hpp"

namespace Beer::Rendering
{
    class Image
    {
        std::shared_ptr<BufferAllocator> allocator;
        ImageAllocation allocation;
        VkImageView defaultView;
    };
} // namespace Beer::Rendering
