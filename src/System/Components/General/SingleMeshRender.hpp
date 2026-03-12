#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Components/General/Transform.hpp"
#include <memory>

namespace Beer::System
{
    class SingleMeshRender : public IRenderComponent
    {
    private:
        std::shared_ptr<Rendering::Material> material;
        std::shared_ptr<Rendering::Mesh> mesh;
        Transform* transform;

    public:
        SingleMeshRender(std::shared_ptr<Rendering::Material> material,
            std::shared_ptr<Rendering::Mesh> mesh,
            Transform* transform)
            : material(material), mesh(mesh), transform(transform)
        {
        }

        void SetMaterial(std::shared_ptr<Rendering::Material> material) { this->material = std::move(material); }
        void SetMesh(std::shared_ptr<Rendering::Mesh> mesh) { this->mesh = std::move(mesh); }
        void SetTransform(Transform* transform) { this->transform = transform; }

        BindHistory Bind(BindMask mask, vk::CommandBuffer commandBuffer, const Rendering::ShaderPassType pass) override;
        virtual bool HasPass(ShaderPassMask mask) override { return material->GetShader()->HasPass(mask); };
        BindMask GetBindMask(BindHistory history) override;

        const Rendering::Shader* GetPrimaryShader() override { return material->GetShader(); }
        const Rendering::Material* GetPrimaryMaterial() override { return material.get(); }
        const Rendering::Mesh* GetPrimaryMesh() override { return mesh.get(); }
    };
} // namespace Beer::System
