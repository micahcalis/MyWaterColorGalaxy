#include "Rendering/Mesh//Mesh.hpp"
#include "Core/Application/Managers/MeshManager.hpp"

namespace Beer::Rendering
{
    std::shared_ptr<Mesh> Mesh::Get(const std::string& name)
    {
        return meshManager->Get(name);
    }
} // namespace Beer::Rendering
