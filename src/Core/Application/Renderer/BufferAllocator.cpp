#define VMA_IMPLEMENTATION
#include "Core/Application/Renderer/BufferAllocator.hpp"

namespace Beer::Core
{
    BufferAllocator::BufferAllocator(const Device& device, const vk::raii::Instance& instance)
        : device(device)
    {
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = *device.GetPhysicalDevice();
        allocatorInfo.device = *device.GetLogicalDevice();
        allocatorInfo.instance = *instance;
        allocatorInfo.vulkanApiVersion = vk::ApiVersion14;

        VkResult result = vmaCreateAllocator(&allocatorInfo, &vmaAllocator);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create VMA allocator!");
        }
    }

    BufferAllocator::~BufferAllocator()
    {
        vmaDestroyAllocator(vmaAllocator);
    }

    const Rendering::BufferAllocation BufferAllocator::CreateBuffer(VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags flags) const
    {
        Rendering::BufferAllocation bufferAlloc;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;
        allocInfo.flags = flags;

        VkResult result = vmaCreateBuffer(vmaAllocator,
            &bufferInfo,
            &allocInfo,
            &bufferAlloc.Buffer,
            &bufferAlloc.Allocation,
            &bufferAlloc.Info);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate buffer!");
        }

        return bufferAlloc;
    }

    const Rendering::BufferAllocation BufferAllocator::CreateStagingBuffer(vk::DeviceSize size) const
    {
        Rendering::BufferAllocation bufferAlloc;

        VkBufferCreateInfo stagingInfo{};
        stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        stagingInfo.size = size;
        stagingInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkResult result = vmaCreateBuffer(vmaAllocator,
            &stagingInfo,
            &allocInfo,
            &bufferAlloc.Buffer,
            &bufferAlloc.Allocation,
            &bufferAlloc.Info);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate buffer!");
        }

        return bufferAlloc;
    }

    void BufferAllocator::DestroyBuffer(const Rendering::BufferAllocation& buffer)
    {
        vmaDestroyBuffer(vmaAllocator, buffer.Buffer, buffer.Allocation);

        if (buffer.Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(vmaAllocator, buffer.Buffer, buffer.Allocation);
        }
    }

    void BufferAllocator::DestroyImage(const Rendering::BufferAllocation& image)
    {
    }
} // namespace Beer::Core
