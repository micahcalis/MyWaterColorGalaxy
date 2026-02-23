#pragma once

#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Buffer/BufferAllocation.hpp"
#include "Core/Application/Renderer/BufferAllocator.hpp"
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include "Core/Application/Utilities/macros.hpp"

namespace Beer::Rendering
{
    class Buffer
    {
    private:
        std::shared_ptr<Core::BufferAllocator> allocator;
        BufferAllocation allocation;
        VkDeviceSize size;

    public:
        ~Buffer();

        static Buffer CreateDeviceLocal(std::shared_ptr<Core::BufferAllocator> allocator,
            VkDeviceSize size,
            VkBufferUsageFlags usage);

        static Buffer CreateStaging(std::shared_ptr<Core::BufferAllocator> allocator,
            VkDeviceSize size);

        static Buffer CreateUniform(std::shared_ptr<Core::BufferAllocator> allocator,
            VkDeviceSize size);

        void Upload(const void* data, size_t size) const;

        void CopyTo(Buffer& dstBuffer,
            const Core::Device& device,
            const Core::FrameResource& frameResource) const;

        [[nodiscard]] VkBuffer
        GetHandle() const { return allocation.Buffer; }

        NO_COPY(Buffer);
        DEFAULT_MOVE(Buffer);

        Buffer(std::shared_ptr<Core::BufferAllocator> allocator,
            BufferAllocation allocation,
            VkDeviceSize size);
    };
} // namespace Beer::Rendering
