#include "Rendering/Buffer/Buffer.hpp"
#include "BufferAllocation.hpp"
#include "Core/Application/Renderer/BufferAllocator.hpp"
#include <iostream>

namespace Beer::Rendering
{
    Buffer Buffer::CreateDeviceLocal(Core::BufferAllocator& allocator,
        VkDeviceSize size,
        VkBufferUsageFlags usage)
    {
        auto tempAlloc = allocator.CreateBuffer(size, usage, VMA_MEMORY_USAGE_AUTO, 0);
        return Buffer(allocator, tempAlloc, size);
    }

    Buffer Buffer::CreateStaging(Core::BufferAllocator& allocator, VkDeviceSize size)
    {
        auto tempAlloc = allocator.CreateStagingBuffer(size);
        return Buffer(allocator, tempAlloc, size);
    }

    Buffer Buffer::CreateUniform(Core::BufferAllocator& allocator, VkDeviceSize size)
    {
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        auto tempAlloc = allocator.CreateBuffer(size,
            usage,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            flags);

        return Buffer(allocator, tempAlloc, size);
    }

    void Buffer::Upload(const void* data, VkDeviceSize size)
    {
        if (size > this->size)
        {
            std::cerr << "Buffer Overflow, can't allocate" << std::endl;
            return;
        }

        if (allocation.Info.pMappedData)
        {
            std::memcpy(allocation.Info.pMappedData, data, size);
        } else
        {
            throw std::runtime_error("Cannot direct upload to unmapped GPU memory!");
        }
    }

    void Buffer::CopyTo(Buffer& dstBuffer,
        const Core::Device& device,
        const Core::FrameResource& frameResource)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = frameResource.GetCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        auto cmdBuffers = device.GetLogicalDevice().allocateCommandBuffers(allocInfo);
        vk::raii::CommandBuffer& copyCommandBuffer = cmdBuffers.front();

        copyCommandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        copyCommandBuffer.copyBuffer(vk::Buffer(allocation.Buffer),
            vk::Buffer(dstBuffer.allocation.Buffer),
            vk::BufferCopy(0, 0, size));

        copyCommandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*copyCommandBuffer;

        device.GetGraphicsQueue().submit(submitInfo, nullptr);
        device.GetGraphicsQueue().waitIdle();
    }

    Buffer::Buffer(Core::BufferAllocator& allocator, BufferAllocation allocation, VkDeviceSize size)
        : allocator(allocator)
        , allocation(allocation)
        , size(size)
    {
    }
} // namespace Beer::Rendering
