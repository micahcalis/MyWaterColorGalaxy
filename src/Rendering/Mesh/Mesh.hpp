#pragma once

#include "Core/Assets/MeshAsset.hpp"
#include "MeshDrawInfo.hpp"
#include "Rendering/Mesh/MeshBuffers.hpp"

namespace Beer::Core
{
    class MeshManager;
}

namespace Beer::Rendering
{
    class Mesh
    {
    private:
        inline static Core::MeshManager* meshManager = nullptr;

        MeshBuffers buffers;
        uint32_t vertexCount;
        uint32_t indexCount;

    public:
        Mesh(MeshBuffers buffers, uint32_t vertexCount, uint32_t indexCount)
            : buffers(std::move(buffers)), vertexCount(vertexCount), indexCount(indexCount)
        {
        }

        static void SetMeshManager(Core::MeshManager* meshManager)
        {
            Mesh::meshManager = meshManager;
        }

        static std::shared_ptr<Mesh> Get(const std::string& name);
        static std::shared_ptr<Mesh> Create(const Core::MeshAsset& asset);

        const MeshBuffers& GetBuffers() const { return buffers; }
        MeshDrawInfo GetDrawInfo() const;
        uint32_t GetVertexCount() const { return vertexCount; }
        uint32_t GetIndexCount() const { return indexCount; }
    };
} // namespace Beer::Rendering
