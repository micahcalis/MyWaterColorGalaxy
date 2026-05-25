#include "System/Galaxy/WatercolorSubPipeline.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"

namespace Beer::System
{
    WatercolorSubPipeline::WatercolorSubPipeline()
    {
        processingBuffers = std::make_unique<Rendering::WatercolorProcessingBuffers>();

        watercolorPostProcessingPass = Rendering::IRenderPass::FetchFromRegister<Rendering::WatercolorPostProcessingPass>(
            Rendering::WC_PROCESSING_PASS,
            processingBuffers.get());
    }

    std::vector<Rendering::IRenderPass*> WatercolorSubPipeline::GetRenderPasses()
    {
        return {watercolorPostProcessingPass};
    }

} // namespace Beer::System
