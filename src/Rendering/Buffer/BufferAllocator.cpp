#include "Rendering/Buffer/BufferAllocator.hpp"
#include "ImageAllocation.hpp"
#include <stdexcept>

namespace Beer::Rendering
{
    BufferAllocator::BufferAllocator(const Core::Device& device, const vk::raii::Instance& instance)
        : device(device), vmaAllocator(nullptr)
    {
        VmaAllocatorCreateInfo allocatorInfo{
            .physicalDevice = *device.GetPhysicalDevice(),
            .device = *device.GetLogicalDevice(),
            .instance = *instance,
            .vulkanApiVersion = vk::ApiVersion14,
        };

        VkResult result = vmaCreateAllocator(&allocatorInfo, &vmaAllocator);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create VMA allocator!");
        }
    }

    BufferAllocator::~BufferAllocator()
    {
        if (vmaAllocator != VK_NULL_HANDLE)
        {
            vmaDestroyAllocator(vmaAllocator);
        }
    }

    BufferAllocation BufferAllocator::CreateBuffer(VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags flags) const
    {
        BufferAllocation bufferAlloc{};

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

    BufferAllocation BufferAllocator::CreateStagingBuffer(vk::DeviceSize size) const
    {
        BufferAllocation bufferAlloc{};

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

    ImageAllocation BufferAllocator::CreateImage2D(uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VmaMemoryUsage memoryUsage,
        uint32_t layerCount) const
    {
        ImageAllocation imageAlloc{};

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = layerCount;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        VkResult result = vmaCreateImage(vmaAllocator,
            &imageInfo,
            &allocInfo,
            &imageAlloc.Image,
            &imageAlloc.Allocation,
            &imageAlloc.Info);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image!");
        }

        return imageAlloc;
    }

    ImageAllocation BufferAllocator::CreateImage3D(uint32_t width,
        uint32_t height,
        uint32_t depth,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VmaMemoryUsage memoryUsage) const
    {
        ImageAllocation imageAlloc{};

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_3D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = depth;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        VkResult result = vmaCreateImage(vmaAllocator,
            &imageInfo,
            &allocInfo,
            &imageAlloc.Image,
            &imageAlloc.Allocation,
            &imageAlloc.Info);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image!");
        }

        return imageAlloc;
    }

    void BufferAllocator::DestroyBuffer(Rendering::BufferAllocation& buffer)
    {
        if (buffer.Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(vmaAllocator, buffer.Buffer, buffer.Allocation);
        }
    }

    void BufferAllocator::DestroyImage(Rendering::ImageAllocation& image)
    {
        if (image.Image != VK_NULL_HANDLE)
        {
            vmaDestroyImage(vmaAllocator, image.Image, image.Allocation);
        }
    }

    void BufferAllocator::DestroyImageView(VkImageView imageView)
    {
        if (imageView != VK_NULL_HANDLE)
        {
            (*device.GetLogicalDevice()).destroyImageView(vk::ImageView(imageView));
        }
    }
} // namespace Beer::Rendering
