#pragma once

#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Buffer/BufferAllocation.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include "Core/Application/Utilities/macros.hpp"
#include "Rendering/Buffer/Image.hpp"

namespace Beer::Rendering
{
    class Buffer
    {
    private:
        // this allocator stuff is pretty cursed, but C objects have weird behaviour so this is fine for now
        inline static std::shared_ptr<BufferAllocator> sharedAllocator = nullptr;
        std::shared_ptr<BufferAllocator> allocator;
        BufferAllocation allocation;
        VkDeviceSize size;

    public:
        ~Buffer();

        static void SetAllocator(std::shared_ptr<BufferAllocator> allocator)
        {
            Buffer::sharedAllocator = allocator;
        }

        static Buffer CreateDeviceLocal(VkDeviceSize size,
            VkBufferUsageFlags usage);

        static Buffer CreateStaging(VkDeviceSize size);

        static Buffer CreateUniform(VkDeviceSize size);

        void Upload(const void* data, size_t size, size_t offset = 0) const;

        void CopyToCmd(Buffer& dstBuffer,
            const Core::Device& device,
            const Core::FrameResource& frameResource,
            const size_t offset = 0) const;

        void QueueCopyTo(Buffer& dstBuffer,
            vk::raii::CommandBuffer& copyCommandBuffer,
            const size_t size,
            const size_t offset = 0) const;

        void QueueStagingTransfer(Image& image,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t size,
            const size_t offset = 0) const;

        void QueueCopyToImage(Image& image,
            vk::raii::CommandBuffer& commandBuffer,
            uint32_t width,
            uint32_t height,
            size_t offset = 0) const;

        [[nodiscard]] VkBuffer
        GetHandle() const
        {
            return allocation.Buffer;
        }

        NO_COPY(Buffer);
        DEFAULT_MOVE(Buffer);

        Buffer(BufferAllocation allocation,
            VkDeviceSize size);
    };
} // namespace Beer::Rendering
