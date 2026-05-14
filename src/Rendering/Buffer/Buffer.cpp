#include "Rendering/Buffer/Buffer.hpp"
#include "BufferAllocation.hpp"
#include "Core/Application/Jobs/BufferUploadJob.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/RenderGarbageCollector.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "Rendering/Buffer/SSBOType.hpp"
#include "vulkan/vulkan.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>

namespace Beer::Rendering
{
    Buffer Buffer::CreateDeviceLocal(VkDeviceSize size, VkBufferUsageFlags usage)
    {
        auto allocation = sharedAllocator->CreateBuffer(size, usage, VMA_MEMORY_USAGE_AUTO, 0);
        BufferData data{};
        data.Size = size;

        return {allocation, data};
    }

    Buffer::~Buffer()
    {
        if (allocator)
        {
            Core::RenderGarbageCollector::Push(
                [savedAllocator = this->allocator,
                    savedAllocation = this->allocation]() mutable {
                    savedAllocator->DestroyBuffer(savedAllocation);
                });
        }
    }

    Buffer Buffer::CreateStaging(VkDeviceSize size)
    {
        auto allocation = sharedAllocator->CreateStagingBuffer(size);
        BufferData data{};
        data.Size = size;

        return {allocation, data};
    }

    Buffer Buffer::CreatePersistent(VkDeviceSize size, VkBufferUsageFlags usage)
    {
        usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

        constexpr VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        auto allocation = sharedAllocator->CreateBuffer(size,
            usage,
            VMA_MEMORY_USAGE_AUTO,
            flags);

        BufferData data{};
        data.Size = size;

        return {allocation, data};
    }

    Buffer Buffer::CreateUniform(VkDeviceSize size)
    {
        constexpr VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        constexpr VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        auto allocation = sharedAllocator->CreateBuffer(size,
            usage,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            flags);

        BufferData data{};
        data.Size = size;

        return {allocation, data};
    }

    Buffer Buffer::CreateDynamic(VkDeviceSize size, VkBufferUsageFlags usage)
    {
        constexpr VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        auto allocation = sharedAllocator->CreateBuffer(size,
            usage,
            VMA_MEMORY_USAGE_AUTO,
            flags);

        BufferData data{};
        data.Size = size;
        data.IsDynamic = true;

        return {allocation, data};
    }

    Buffer Buffer::CreateSSBO(VkDeviceSize size, SSBOType type)
    {
        constexpr VkBufferUsageFlags hybridUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT
            | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        constexpr VkBufferUsageFlags consistentUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VkBufferUsageFlags usage = 0;

        if (type == SSBOType::Hybrid)
        {
            usage = hybridUsage;
        }

        if (type == SSBOType::Consistent)
        {
            usage = consistentUsage;
        }

        if (type != SSBOType::Consistent)
        {
            return CreatePersistent(size, usage);
        } else
        {
            return CreateDeviceLocal(size, usage);
        }
    }

    Buffer Buffer::CreateReadback(VkDeviceSize size)
    {
        constexpr VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        constexpr VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        auto allocation = sharedAllocator->CreateBuffer(size,
            usage,
            VMA_MEMORY_USAGE_AUTO,
            flags);

        BufferData data{};
        data.Size = size;

        return {allocation, data};
    }

    void Buffer::Upload(const void* data, size_t size, size_t offset) const
    {
        if (size + offset > this->data.Size)
        {
            std::cerr << "Buffer Overflow, can't allocate" << '\n';
            return;
        }

        if (allocation.Info.pMappedData != nullptr)
        {
            void* destination = static_cast<char*>(allocation.Info.pMappedData) + offset;
            std::memcpy(destination, data, size);
        } else
        {
            throw std::runtime_error("Cannot direct upload to unmapped GPU memory!");
        }
    }

    void Buffer::Invalidate()
    {
        vmaInvalidateAllocation(allocator->GetAllocator(), allocation.Allocation, 0, VK_WHOLE_SIZE);
    }

    void Buffer::CopyToCmd(Buffer& dstBuffer,
        const Core::Device& device,
        const Core::FrameResource& frameResource,
        const size_t offset) const
    {
        vk::raii::CommandBuffer copyCommandBuffer = Core::CommandBufferUtilities::BeginSingleTimeCommands(frameResource, device);

        copyCommandBuffer.copyBuffer(vk::Buffer(allocation.Buffer),
            vk::Buffer(dstBuffer.allocation.Buffer),
            vk::BufferCopy(0, 0, data.Size));

        Core::CommandBufferUtilities::EndSingleTimeCommands(copyCommandBuffer, device);
    }

    void Buffer::QueueCopyTo(Buffer& dstBuffer,
        vk::raii::CommandBuffer& copyCommandBuffer,
        const size_t size,
        const size_t offset) const
    {
        copyCommandBuffer.copyBuffer(vk::Buffer(allocation.Buffer),
            vk::Buffer(dstBuffer.allocation.Buffer),
            vk::BufferCopy(offset, 0, size));
    }

    void Buffer::QueueStagingTransfer(Image& image,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t size,
        const size_t offset) const
    {
        image.QueueTransitionLayout(image.GetHandle(),
            image.GetData(),
            commandBuffer,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal);

        const vk::Extent3D extent = image.GetExtent();
        QueueCopyToImage(image, commandBuffer, extent.width, extent.height, offset);

        image.QueueTransitionLayout(image.GetHandle(),
            image.GetData(),
            commandBuffer,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    void Buffer::QueueCopyToImage(Image& image,
        vk::raii::CommandBuffer& commandBuffer,
        uint32_t width,
        uint32_t height,
        size_t offset) const
    {
        vk::BufferImageCopy region{};
        region.bufferOffset = offset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource = image.GetData().AspectMask;
        region.imageOffset = vk::Offset3D(0, 0, 0);
        region.imageExtent = vk::Extent3D(width, height, 1);
        region.imageSubresource.layerCount = 1;

        commandBuffer.copyBufferToImage(GetHandle(),
            image.GetHandle(),
            vk::ImageLayout::eTransferDstOptimal,
            {region});
    }

    Buffer::Buffer(BufferAllocation allocation, BufferData data)
        : allocator(sharedAllocator), allocation(allocation), data(data)
    {
    }

    void Buffer::UploadAsync(std::shared_ptr<Buffer> buffer, void* data, size_t size, size_t offset)
    {
        if (size + offset > buffer->data.Size)
        {
            std::cerr << "Buffer Overflow, can't allocate" << '\n';
            return;
        }

        if (buffer->allocation.Info.pMappedData != nullptr)
        {
            throw std::runtime_error("Cannot async upload to unmapped GPU memory!");
        }

        if (uploadManager == nullptr)
        {
            throw std::runtime_error("Cannot async upload because upload manager is null!");
        }

        std::unique_ptr<Core::BufferUploadJob> uploadJob = std::make_unique<Core::BufferUploadJob>(
            buffer,
            data,
            size);

        uploadManager->AddJob(std::move(uploadJob));
    }
} // namespace Beer::Rendering

