#include "Core/Application/Utilities/MeshUtilities.hpp"
#include "Core/Application/Jobs/BufferUploadJob.hpp"
#include "Core/Application/Jobs/IUploadJob.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include <memory>

namespace Beer::Core
{
    const VkBufferUsageFlags VERTEX_ATTRIB_FLAGS = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    const VkBufferUsageFlags INDEX_FLAGS = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    std::vector<std::unique_ptr<IUploadJob>> MeshUtilities::GetMeshAssetUploads(MeshAsset& meshAsset,
        Rendering::MeshBuffers& buffers,
        const std::shared_ptr<Rendering::BufferAllocator>& bufferAllocator)
    {
        std::vector<std::unique_ptr<IUploadJob>> uploadJobs;

        // TryEmplaceData(uploadJobs,
        //     meshAsset.Positions.data(),
        //     meshAsset.GetPositionsSize(),
        //     buffers.PositionBuffer,
        //     bufferAllocator,
        //     true);

        // TryEmplaceData(uploadJobs,
        //     meshAsset.UVs.data(),
        //     meshAsset.GetUvsSize(),
        //     buffers.UvBuffer,
        //     bufferAllocator,
        //     true);

        // TryEmplaceData(uploadJobs,
        //     meshAsset.VertexColors.data(),
        //     meshAsset.GetColorsSize(),
        //     buffers.ColorBuffer,
        //     bufferAllocator,
        //     true);

        // TryEmplaceData(uploadJobs,
        //     meshAsset.Indices.data(),
        //     meshAsset.GetIndicesSize(),
        //     buffers.IndexBuffer,
        //     bufferAllocator,
        //     false);

        return uploadJobs;
    }

    void MeshUtilities::TryEmplaceData(std::vector<std::unique_ptr<IUploadJob>>& uploadJobs,
        void* data,
        vk::DeviceSize bufferSize,
        std::shared_ptr<Rendering::Buffer>& buffer,
        const std::shared_ptr<Rendering::BufferAllocator>& bufferAllocator,
        bool isVertAttrib)
    {
        // if (bufferSize == 0)
        //     return;

        // VkBufferUsageFlags flags = isVertAttrib ? VERTEX_ATTRIB_FLAGS : INDEX_FLAGS;

        // buffer = std::make_shared<Rendering::Buffer>(
        //     Rendering::Buffer::CreateDeviceLocal(bufferAllocator, bufferSize, flags));

        // uploadJobs.emplace_back(std::make_unique<BufferUploadJob>(
        //     buffer, data, bufferSize));
    }
} // namespace Beer::Core
