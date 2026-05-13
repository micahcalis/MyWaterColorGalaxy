#pragma once

#include "System/Components/General/MeshRenderComponent.hpp"
#include "Rendering/Shader/Globals/ModelTransformData.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Delegates/Delegate.hpp"
#include "Transform.hpp"

namespace Beer::System
{
    class MultipleMeshRender : public MeshRenderComponent
    {
        Transform* rootTransform;
        std::shared_ptr<Rendering::Material> material;
        std::shared_ptr<Rendering::Mesh> mesh;
        Function<std::vector<Rendering::ModelTransformData>> getModelTransformData;
        Layer* layer;

    public:
        MultipleMeshRender(Transform* rootTransform,
            std::shared_ptr<Rendering::Material> material,
            std::shared_ptr<Rendering::Mesh> mesh,
            Layer* layer)
            : rootTransform(rootTransform), material(material), mesh(mesh), layer(layer)
        {
        }

        void SetMaterial(std::shared_ptr<Rendering::Material> material) { this->material = std::move(material); }
        void SetMesh(std::shared_ptr<Rendering::Mesh> mesh) { this->mesh = std::move(mesh); }
        void SetModelDataFunction(Function<std::vector<Rendering::ModelTransformData>> getModelTransformData) { this->getModelTransformData = getModelTransformData; }
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
            return glm::dot(rootTransform->Position - cameraPos, cameraForward);
        }
    };
} // namespace Beer::System
