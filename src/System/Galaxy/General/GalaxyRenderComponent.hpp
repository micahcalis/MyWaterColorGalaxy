#pragma once

#include "GalaxyContainer.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Components/General/MeshRenderComponent.hpp"

namespace Beer::System
{
    class GalaxyRenderComponent : public MeshRenderComponent
    {
    private:
        GalaxyContainer* container = nullptr;

    public:
        GalaxyRenderComponent(GalaxyContainer* container)
            : container(container)
        {
        }

        BindHistory Bind(BindMask mask,
            Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            const Rendering::ShaderPassType pass)
            override;

        bool HasPass(Rendering::ShaderPassType pass) const override;
        Layer GetLayer() const override { return Layer::Default; }
        BindMask GetBindMask(const BindHistory& history) const override;

        const Rendering::Shader* GetPrimaryShader() const override { return nullptr; }
        const Rendering::Material* GetPrimaryMaterial() const override { return nullptr; }
        const Rendering::Mesh* GetPrimaryMesh() const override { return nullptr; }
    };
} // namespace Beer::System
