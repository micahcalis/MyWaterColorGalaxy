#include "Core/Application/Jobs/MeshUploadJob.hpp"
#include "Rendering/Mesh/MeshBufferType.hpp"
#include "Rendering/Mesh/MeshBuffers.hpp"

namespace Beer::Core
{
    void MeshUploadJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        const Rendering::MeshBuffers& buffers = mesh->GetBuffers();

        size_t currentOffset = offset;

        for (int i = 0; i < static_cast<int>(Rendering::MeshBufferType::Count); i++)
        {
            Rendering::MeshBufferType type = static_cast<Rendering::MeshBufferType>(i);

            if (buffers.HasBuffer(type))
            {
                size_t size = meshAsset.GetBufferSize(type);
                stagingBuffer->Upload(meshAsset.GetBufferData(type), size, currentOffset);
                stagingBuffer->QueueCopyTo(*buffers.GetBuffer(type), commandBuffer, size, currentOffset);
                currentOffset += size;
            }
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
