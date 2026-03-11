#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include <memory>

namespace Beer::System
{
    class RenderComponent
    {
    private:
        std::shared_ptr<Rendering::Mesh> mesh;
        std::shared_ptr<Rendering::Material> material;

    public:
        RenderComponent(std::shared_ptr<Rendering::Mesh> mesh, std::shared_ptr<Rendering::Material> material)
            : mesh(std::move(mesh)), material(std::move(material))
        {
        }
    };
} // namespace Beer::System
