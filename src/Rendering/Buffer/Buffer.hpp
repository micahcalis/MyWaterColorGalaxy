#pragma once

#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Buffer/BufferAllocation.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include "Core/Application/Utilities/macros.hpp"

namespace Beer::Rendering
{
    class Buffer
    {
    private:
        std::shared_ptr<BufferAllocator> allocator;
        BufferAllocation allocation;
        VkDeviceSize size;

    public:
        ~Buffer();

        static Buffer CreateDeviceLocal(std::shared_ptr<BufferAllocator> allocator,
            VkDeviceSize size,
            VkBufferUsageFlags usage);

        static Buffer CreateStaging(std::shared_ptr<BufferAllocator> allocator,
            VkDeviceSize size);

        static Buffer CreateUniform(std::shared_ptr<BufferAllocator> allocator,
            VkDeviceSize size);

        void Upload(const void* data, size_t size, size_t offset = 0) const;

        void CopyToCmd(Buffer& dstBuffer,
            const Core::Device& device,
            const Core::FrameResource& frameResource,
            const size_t offset = 0) const;

        void QueueCopyTo(Buffer& dstBuffer,
            vk::raii::CommandBuffer& copyCommandBuffer,
            const size_t size,
            const size_t offset = 0) const;

        [[nodiscard]] VkBuffer
        GetHandle() const { return allocation.Buffer; }

        NO_COPY(Buffer);
        DEFAULT_MOVE(Buffer);

        Buffer(std::shared_ptr<Rendering::BufferAllocator> allocator,
            BufferAllocation allocation,
            VkDeviceSize size);
    };
} // namespace Beer::Rendering
