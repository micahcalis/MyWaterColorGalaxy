#include "Rendering/Buffer/Buffer.hpp"
#include "BufferAllocation.hpp"
#include <iostream>

namespace Beer::Rendering
{
    Buffer Buffer::CreateDeviceLocal(const Core::BufferAllocator& allocator,
        VkDeviceSize size,
        VkBufferUsageFlags usage)
    {
        auto tempAlloc = allocator.CreateBuffer(size, usage, VMA_MEMORY_USAGE_AUTO);
        return Buffer(allocator, tempAlloc, size);
    }

    Buffer Buffer::CreateStaging(const Core::BufferAllocator& allocator, VkDeviceSize size)
    {
        auto tempAlloc = allocator.CreateStagingBuffer(size);
        return Buffer(allocator, tempAlloc, size);
    }

    Buffer Buffer::CreateUniform(const Core::BufferAllocator& allocator, VkDeviceSize size)
    {
        auto tempAlloc = allocator.CreateBuffer(size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

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
            memcpy(allocation.Info.pMappedData, data, size);
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

    Buffer::Buffer(const Core::BufferAllocator& allocator, BufferAllocation allocation, VkDeviceSize size)
        : allocator(allocator)
        , allocation(allocation)
        , size(size)
    {
    }
} // namespace Beer::Rendering
