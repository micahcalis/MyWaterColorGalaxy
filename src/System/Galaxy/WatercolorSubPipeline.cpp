#include "System/Galaxy/WatercolorSubPipeline.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/RenderPasses/Watercolor/BlitMainColorPass.hpp"
#include "Rendering/RenderPasses/Watercolor/GaussianBlurPass.hpp"
#include "Rendering/RenderPasses/Watercolor/OffsetEdgeBlurPass.hpp"
#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"
#include "Rendering/RenderPasses/Watercolor/WatercolorProcessingBuffers.hpp"

namespace Beer::System
{
    static const uint32_t BLUR_DEPTH = 21;
    static const float BLUR_SPREAD = 20.0f;

    static const uint32_t EDGE_BLUR_DEPTH = 5;
    static const float EDGE_BLUR_SPREAD = 10.0f;
    static const float EDGE_DEPTH_THRESHOLD = 10.0f;

    WatercolorSubPipeline::WatercolorSubPipeline()
    {
        processingBuffers = std::make_unique<Rendering::WatercolorProcessingBuffers>();

        waterColorLowerResBlitPass = Rendering::IRenderPass::FetchFromRegister<Rendering::BlitMainColorPass>(
            Rendering::WC_BLIT_PASS,
            Rendering::WC_COLORBLIT_TEX_A,
            Rendering::WC_BLIT_RESOLUTION,
            Rendering::RenderPassEvent::WATERCOLOR,
            0);

        blurHorizontalPass = Rendering::IRenderPass::FetchFromRegister<Rendering::GaussianBlurPass>(
            Rendering::WC_BLUR_PASS_A,
            Rendering::WC_COLORBLIT_TEX_A,
            Rendering::WC_COLORBLIT_TEX_B,
            Rendering::WC_BLIT_RESOLUTION,
            Rendering::GaussDirection::Horizontal,
            BLUR_DEPTH,
            BLUR_SPREAD,
            Rendering::RenderPassEvent::WATERCOLOR,
            1);

        blurVerticalPass = Rendering::IRenderPass::FetchFromRegister<Rendering::GaussianBlurPass>(
            Rendering::WC_BLUR_PASS_B,
            Rendering::WC_COLORBLIT_TEX_B,
            Rendering::WC_COLORBLIT_TEX_A,
            Rendering::WC_BLIT_RESOLUTION,
            Rendering::GaussDirection::Vertical,
            BLUR_DEPTH,
            BLUR_SPREAD,
            Rendering::RenderPassEvent::WATERCOLOR,
            2);

        edgeBlurHorizontalPass = Rendering::IRenderPass::FetchFromRegister<Rendering::OffsetEdgeBlurPass>(
            Rendering::WC_EDGE_BLUR_PASS_A,
            Rendering::GaussDirection::Horizontal,
            EDGE_BLUR_DEPTH,
            EDGE_BLUR_SPREAD,
            EDGE_DEPTH_THRESHOLD,
            Rendering::RenderPassEvent::WATERCOLOR,
            3);

        edgeBlurVerticalPass = Rendering::IRenderPass::FetchFromRegister<Rendering::OffsetEdgeBlurPass>(
            Rendering::WC_EDGE_BLUR_PASS_B,
            Rendering::GaussDirection::Vertical,
            EDGE_BLUR_DEPTH,
            EDGE_BLUR_SPREAD,
            EDGE_DEPTH_THRESHOLD,
            Rendering::RenderPassEvent::WATERCOLOR,
            4);

        watercolorPostProcessingPass = Rendering::IRenderPass::FetchFromRegister<Rendering::WatercolorPostProcessingPass>(
            Rendering::WC_PROCESSING_PASS,
            processingBuffers.get());
    }

    std::vector<Rendering::IRenderPass*> WatercolorSubPipeline::GetRenderPasses()
    {
        return {waterColorLowerResBlitPass,
            blurHorizontalPass,
            blurVerticalPass,
            edgeBlurHorizontalPass,
            edgeBlurVerticalPass,
            watercolorPostProcessingPass};
    }

} // namespace Beer::System
