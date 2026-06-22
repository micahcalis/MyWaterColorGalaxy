#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"

namespace Beer::Rendering
{
    class FilterPostProcessingPass : public IRenderPass
    {
    private:
        std::shared_ptr<Rendering::Material> postProcessingMaterial = nullptr;

    public:
        FilterPostProcessingPass(std::shared_ptr<Material> postProcessingMateriial,
            RenderPassEvent event,
            int32_t eventOffset = 0)
            : postProcessingMaterial(postProcessingMateriial)
            , IRenderPass("FilterPostProcessingPass", static_cast<int32_t>(event) + eventOffset)
        {
        }

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

        bool BlitsMainTarget() const override
        {
            return true;
        }
    };
} // namespace Beer::Rendering
