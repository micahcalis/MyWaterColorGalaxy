#pragma once

#include "MeshRenderComponent.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Components/General/Transform.hpp"
#include <memory>

namespace Beer::System
{
    class SingleMeshRender : public MeshRenderComponent
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

        float GetPlanarDist(const glm::vec3 cameraPos, const glm::vec3 cameraForward) const override
        {
            return glm::dot(transform->Position - cameraPos, cameraForward);
        }
    };
} // namespace Beer::System
