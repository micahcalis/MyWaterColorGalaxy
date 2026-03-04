#include "Rendering/Mesh/MeshBuffers.hpp"
#include "Core/Assets/MeshAsset.hpp"
#include "MeshBufferType.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include <memory>

namespace Beer::Rendering
{
    const VkBufferUsageFlags VERTEX_ATTRIB_FLAGS = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    const VkBufferUsageFlags INDEX_FLAGS = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    MeshBuffers::MeshBuffers(const Core::MeshAsset& meshAsset)
    {
        for (int i = 0; i < static_cast<int>(MeshBufferType::Count); i++)
        {
            MeshBufferType type = static_cast<MeshBufferType>(i);

            if (meshAsset.GetBufferSize(type) > 0)
            {
                vertexBuffers[type] = std::make_shared<Rendering::Buffer>(
                    Rendering::Buffer::CreateDeviceLocal(
                        meshAsset.GetBufferSize(type),
                        VERTEX_ATTRIB_FLAGS));
            } else
            {
                vertexBuffers[type] = std::make_shared<Rendering::Buffer>(
                    Rendering::Buffer::CreateDeviceLocal(
                        Core::MeshAsset::GetImaginarySize(type, meshAsset.GetVertexCount()),
                        VERTEX_ATTRIB_FLAGS));
            }
        }

        if (meshAsset.GetIndexCount() > 0)
        {
            IndexBuffer = std::make_shared<Rendering::Buffer>(
                Rendering::Buffer::CreateDeviceLocal(
                    meshAsset.GetIndicesSize(),
                    INDEX_FLAGS));
        }
    }
} // namespace Beer::Rendering
