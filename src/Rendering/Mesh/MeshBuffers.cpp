#include "Rendering/Mesh/MeshBuffers.hpp"
#include "Core/Assets/MeshAsset.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include <memory>

namespace Beer::Rendering
{
    const VkBufferUsageFlags VERTEX_ATTRIB_FLAGS = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    const VkBufferUsageFlags INDEX_FLAGS = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    MeshBuffers::MeshBuffers(const Core::MeshAsset& meshAsset,
        const std::shared_ptr<BufferAllocator>& bufferAllocator)
    {
        if (meshAsset.GetVertexCount() > 0)
        {
            PositionBuffer = std::make_shared<Rendering::Buffer>(
                Rendering::Buffer::CreateDeviceLocal(bufferAllocator,
                    meshAsset.GetPositionsSize(),
                    VERTEX_ATTRIB_FLAGS));
        }

        if (meshAsset.GetUvsSize() > 0)
        {
            UvBuffer = std::make_shared<Rendering::Buffer>(
                Rendering::Buffer::CreateDeviceLocal(bufferAllocator,
                    meshAsset.GetUvsSize(),
                    VERTEX_ATTRIB_FLAGS));
        }

        if (meshAsset.GetColorsSize() > 0)
        {
            ColorBuffer = std::make_shared<Rendering::Buffer>(
                Rendering::Buffer::CreateDeviceLocal(bufferAllocator,
                    meshAsset.GetColorsSize(),
                    VERTEX_ATTRIB_FLAGS));
        }

        if (meshAsset.GetIndexCount() > 0)
        {
            IndexBuffer = std::make_shared<Rendering::Buffer>(
                Rendering::Buffer::CreateDeviceLocal(bufferAllocator,
                    meshAsset.GetIndicesSize(),
                    INDEX_FLAGS));
        }
    }
} // namespace Beer::Rendering
