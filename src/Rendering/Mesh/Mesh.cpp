#include "Rendering/Mesh//Mesh.hpp"
#include "Core/Application/Managers/MeshManager.hpp"

namespace Beer::Rendering
{
    std::shared_ptr<Mesh> Mesh::Get(const std::string& name)
    {
        return meshManager->Get(name);
    }

    void Mesh::Bind(vk::CommandBuffer commandBuffer, const MeshBufferOrder bufferOrder, bool& canIndex) const
    {
        std::vector<vk::Buffer> activeBuffers;
        const Rendering::MeshBuffers& buffers = GetBuffers();

        for (int i = 0; i < bufferOrder.GetSize(); i++)
        {
            const Rendering::MeshBufferType type = bufferOrder.GetElement(i);

            if (buffers.HasBuffer(type))
                activeBuffers.push_back(buffers.GetBuffer(type)->GetHandle());
        }

        std::vector<vk::DeviceSize> offsets(activeBuffers.size(), 0);

        commandBuffer.bindVertexBuffers(0, activeBuffers, offsets);

        canIndex = buffers.HasIndex();

        if (!canIndex)
            return;

        commandBuffer.bindIndexBuffer(buffers.IndexBuffer->GetHandle(), 0, vk::IndexType::eUint32);
    }
} // namespace Beer::Rendering
