#include "Rendering/Mesh//Mesh.hpp"
#include "Core/Application/Managers/MeshManager.hpp"
#include "MeshDrawInfo.hpp"

namespace Beer::Rendering
{
    std::shared_ptr<Mesh> Mesh::Get(const std::string& name)
    {
        return meshManager->Get(name);
    }

    MeshDrawInfo Mesh::GetDrawInfo() const
    {
        return MeshDrawInfo(buffers.HasIndex(),
            vertexCount,
            indexCount);
    }
} // namespace Beer::Rendering
