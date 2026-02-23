#pragma once

#include "Rendering/Buffer/ImageAllocation.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Buffer/ImageAllocation.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Utilities/macros.hpp"

namespace Beer::Rendering
{
    class Image
    {
    private:
        std::shared_ptr<BufferAllocator> allocator;
        ImageAllocation allocation;
        VkImageView defaultView;
        vk::Extent3D extent;
        VkFormat format;

    public:
        ~Image();

        static Image CreateImage2D(std::shared_ptr<BufferAllocator> allocator,
            uint32_t width,
            uint32_t height,
            VkFormat format,
            const Core::Device& device);

        [[nodiscard]] VkImage GetHandle() const { return allocation.Image; }
        VkImageView GetDefaultView() const { return defaultView; }
        vk::Extent3D GetExtent() const { return extent; }
        VkFormat GetFormat() const { return format; }

        NO_COPY(Image);
        DEFAULT_MOVE(Image);

    private:
        Image(std::shared_ptr<BufferAllocator> allocator,
            ImageAllocation allocation,
            VkImageView defaultView,
            vk::Extent3D extent,
            VkFormat format);
    };
} // namespace Beer::Rendering
