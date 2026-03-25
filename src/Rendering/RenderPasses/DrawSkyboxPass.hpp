#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "System/Components/General/Transform.hpp"
#include <memory>

namespace Beer::Rendering
{
    class DrawSkyboxPass : public IRenderPass
    {
    private:
        std::shared_ptr<Material> skyboxMaterial;
        std::shared_ptr<Mesh> cubeMesh;
        System::Transform skyboxTransform{};

    public:
        DrawSkyboxPass();

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
