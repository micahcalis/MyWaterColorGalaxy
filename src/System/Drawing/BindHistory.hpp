#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Shader/Shader.hpp"

namespace Beer::System
{
    struct BindHistory
    {
    private:
        const Rendering::Shader* shader;
        const Rendering::Material* material;
        const Rendering::Mesh* mesh;

    public:
        BindHistory(const Rendering::Shader* shader,
            const Rendering::Material* material,
            const Rendering::Mesh* mesh)
            : shader(shader), material(material), mesh(mesh)
        {
        }

        [[nodiscard]] const Rendering::Shader* GetShader() const { return shader; }
        [[nodiscard]] const Rendering::Material* GetMaterial() const { return material; }
        [[nodiscard]] const Rendering::Mesh* GetMesh() const { return mesh; }
    };
} // namespace Beer::System
