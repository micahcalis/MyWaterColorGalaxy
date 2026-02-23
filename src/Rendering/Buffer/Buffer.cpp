#include "Rendering/Buffer/Buffer.hpp"
#include "BufferAllocation.hpp"
#include "Core/Application/Renderer/BufferAllocator.hpp"
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include <iostream>
#include <utility>

namespace Beer::Rendering
{

    Buffer Buffer::CreateDeviceLocal(std::shared_ptr<Core::BufferAllocator> allocator, VkDeviceSize size, VkBufferUsageFlags usage)
    {
        auto allocation = allocator->CreateBuffer(size, usage, VMA_MEMORY_USAGE_AUTO, 0);
        return {std::move(allocator), allocation, size};
    }

    Buffer::~Buffer()
    {
        if (allocator)
        {
            allocator->DestroyBuffer(allocation);
        }
    }

    Buffer Buffer::CreateStaging(std::shared_ptr<Core::BufferAllocator> allocator, VkDeviceSize size)
    {
        auto allocation = allocator->CreateStagingBuffer(size);
        return {std::move(allocator), allocation, size};
    }

    Buffer Buffer::CreateUniform(std::shared_ptr<Core::BufferAllocator> allocator, VkDeviceSize size)
    {
        constexpr VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        constexpr VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        auto allocation = allocator->CreateBuffer(size,
            usage,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            flags);

        return {std::move(allocator), allocation, size};
    }

    void Buffer::Upload(const void* data, VkDeviceSize size) const
    {
        if (size > this->size)
        {
            std::cerr << "Buffer Overflow, can't allocate" << '\n';
            return;
        }

        if (allocation.Info.pMappedData != nullptr)
        {
            std::memcpy(allocation.Info.pMappedData, data, size);
        } else
        {
            throw std::runtime_error("Cannot direct upload to unmapped GPU memory!");
        }
    }

    void Buffer::CopyTo(Buffer& dstBuffer,
        const Core::Device& device,
        const Core::FrameResource& frameResource) const
    {
        vk::raii::CommandBuffer copyCommandBuffer = Core::CommandBufferUtilities::BeginSingleTimeCommands(frameResource, device);

        copyCommandBuffer.copyBuffer(vk::Buffer(allocation.Buffer),
            vk::Buffer(dstBuffer.allocation.Buffer),
            vk::BufferCopy(0, 0, size));

        Core::CommandBufferUtilities::EndSingleTimeCommands(copyCommandBuffer, device);
    }

    Buffer::Buffer(std::shared_ptr<Core::BufferAllocator> allocator, BufferAllocation allocation, VkDeviceSize size)
        : allocator(std::move(allocator))
        , allocation(allocation)
        , size(size)
    {
    }
} // namespace Beer::Rendering
