#pragma once

#include "System/Components/General/IRenderComponent.hpp"

namespace Beer::System
{
    class MultipleMeshRender : public IRenderComponent
    {
        std::shared_ptr<Rendering::Material> material;
        std::shared_ptr<Rendering::Mesh> mesh;
        std::vector<Transform>& transforms;
        Layer* layer;

    public:
        MultipleMeshRender(std::shared_ptr<Rendering::Material> material,
            std::shared_ptr<Rendering::Mesh> mesh,
            std::vector<Transform>& transforms,
            Layer* layer)
            : material(material), mesh(mesh), transforms(transforms), layer(layer)
        {
        }

        void SetMaterial(std::shared_ptr<Rendering::Material> material) { this->material = std::move(material); }
        void SetMesh(std::shared_ptr<Rendering::Mesh> mesh) { this->mesh = std::move(mesh); }
        void SetTransforms(std::vector<Transform>& transforms) { this->transforms = transforms; }
        void SetLayer(Layer* layer) { this->layer = layer; }

        BindHistory Bind(BindMask mask,
            Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            const Rendering::ShaderPassType pass) override;

        virtual bool HasPass(Rendering::ShaderPassType pass) const override { return material->GetShader()->HasPass(pass); }
        virtual Layer GetLayer() const override { return *layer; }
        BindMask GetBindMask(const BindHistory& history) const override;

        const Rendering::Shader* GetPrimaryShader() const override { return material->GetShader(); }
        const Rendering::Material* GetPrimaryMaterial() const override { return material.get(); }
        const Rendering::Mesh* GetPrimaryMesh() const override { return mesh.get(); }
    };
} // namespace Beer::System
