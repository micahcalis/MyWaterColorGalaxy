#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
namespace Beer::System
{
    struct RenderComponent
    {
        std::shared_ptr<Rendering::Mesh> mesh;
        std::shared_ptr<Rendering::Material> material;
    };
} // namespace Beer::System
