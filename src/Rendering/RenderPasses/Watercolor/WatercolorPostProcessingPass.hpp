#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Watercolor/WatercolorProcessingBuffers.hpp"
#include "WatercolorProcessingBuffers.hpp"

namespace Beer::Rendering
{
    class WatercolorPostProcessingPass : public IRenderPass
    {
    private:
        WatercolorProcessingBuffers* processingBuffers = nullptr;
        std::shared_ptr<Rendering::Material> postProcessingMaterial = nullptr;

    public:
        WatercolorPostProcessingPass(WatercolorProcessingBuffers* processingBuffers);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

        bool BlitsMainTarget() const override
        {
            return true;
        }
    };
} // namespace Beer::Rendering
