#include "Core/Application/Jobs/MeshUploadJob.hpp"
#include "Rendering/Mesh/MeshBuffers.hpp"

namespace Beer::Core
{
    void MeshUploadJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        const Rendering::MeshBuffers& buffers = mesh->GetBuffers();

        size_t currentOffset = offset;

        if (buffers.HasPositions())
        {
            size_t size = meshAsset.GetPositionsSize();
            stagingBuffer->Upload(meshAsset.Positions.data(), size, currentOffset);
            stagingBuffer->QueueCopyTo(*buffers.PositionBuffer, commandBuffer, size, currentOffset);
            currentOffset += size;
        }

        if (buffers.HasUv())
        {
            size_t size = meshAsset.GetUvsSize();
            stagingBuffer->Upload(meshAsset.UVs.data(), size, currentOffset);
            stagingBuffer->QueueCopyTo(*buffers.UvBuffer, commandBuffer, size, currentOffset);
            currentOffset += size;
        }

        if (buffers.HasColor())
        {
            size_t size = meshAsset.GetColorsSize();
            stagingBuffer->Upload(meshAsset.VertexColors.data(), size, currentOffset);
            stagingBuffer->QueueCopyTo(*buffers.ColorBuffer, commandBuffer, size, currentOffset);
            currentOffset += size;
        }

        if (buffers.HasIndex())
        {
            size_t size = meshAsset.GetIndicesSize();
            stagingBuffer->Upload(meshAsset.Indices.data(), size, currentOffset);
            stagingBuffer->QueueCopyTo(*buffers.IndexBuffer, commandBuffer, size, currentOffset);
            currentOffset += size;
        }
    }
} // namespace Beer::Core
