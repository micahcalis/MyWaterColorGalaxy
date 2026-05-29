#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/RenderPasses/Watercolor/DynamicCanvasUpdater.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Watercolor/WatercolorProcessingBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "WatercolorProcessingBuffers.hpp"

namespace Beer::Rendering
{
    class WatercolorPostProcessingPass : public IRenderPass
    {
    private:
        WatercolorProcessingBuffers* processingBuffers = nullptr;
        std::shared_ptr<Rendering::Material> postProcessingMaterial = nullptr;
        std::unique_ptr<DynamicCanvasUpdater> dynamicCanvasUpdater = nullptr;
        std::shared_ptr<Rendering::Texture2D> paperSourceTex = nullptr;
        std::shared_ptr<Rendering::Texture2D> paperSlopeMap = nullptr;
        std::shared_ptr<Rendering::ComputeContext> slopeContext = nullptr;

    public:
        WatercolorPostProcessingPass(WatercolorProcessingBuffers* processingBuffers);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

        bool BlitsMainTarget() const override
        {
            return true;
        }

    private:
        void InitializePaperSlopeMap();
    };
} // namespace Beer::Rendering
