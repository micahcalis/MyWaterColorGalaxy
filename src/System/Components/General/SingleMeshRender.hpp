#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
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
        Layer* layer;

    public:
        SingleMeshRender(std::shared_ptr<Rendering::Material> material,
            std::shared_ptr<Rendering::Mesh> mesh,
            Transform* transform,
            Layer* layer)
            : material(material), mesh(mesh), transform(transform), layer(layer)
        {
        }

        void SetMaterial(std::shared_ptr<Rendering::Material> material) { this->material = std::move(material); }
        void SetMesh(std::shared_ptr<Rendering::Mesh> mesh) { this->mesh = std::move(mesh); }
        void SetTransform(Transform* transform) { this->transform = transform; }
        void SetLayer(Layer* layer) { this->layer = layer; }

        BindHistory Bind(BindMask mask, vk::CommandBuffer commandBuffer, const Rendering::ShaderPassType pass) override;

        virtual bool HasPass(Rendering::ShaderPassType pass) const override { return material->GetShader()->HasPass(pass); }
        virtual Layer GetLayer() const override { return *layer; }
        BindMask GetBindMask(const BindHistory& history) const override;

        const Rendering::Shader* GetPrimaryShader() const override { return material->GetShader(); }
        const Rendering::Material* GetPrimaryMaterial() const override { return material.get(); }
        const Rendering::Mesh* GetPrimaryMesh() const override { return mesh.get(); }
    };
} // namespace Beer::System
