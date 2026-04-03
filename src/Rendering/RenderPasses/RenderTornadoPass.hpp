#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include <memory>
namespace Beer::Rendering
{
    class RenderTornadoPass final : public IRenderPass
    {
    private:
        PhaseBuffer* particleBuffer;
        std::shared_ptr<Material> particleMaterial;
        std::shared_ptr<Mesh> cubeMesh;

    public:
        RenderTornadoPass(const uint32_t intEvent);

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };

} // namespace Beer::Rendering
