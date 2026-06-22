#include "System/Galaxy/WatercolorSubPipeline.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/RenderPasses/Watercolor/BlitMainColorPass.hpp"
#include "Rendering/RenderPasses/Watercolor/FilterPostProcessingPass.hpp"
#include "Rendering/RenderPasses/Watercolor/GaussianBlurPass.hpp"
#include "Rendering/RenderPasses/Watercolor/OffsetEdgeBlurPass.hpp"
#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"
#include "Rendering/RenderPasses/Watercolor/WatercolorProcessingBuffers.hpp"

namespace Beer::System
{
    static const uint32_t BLUR_DEPTH = 21;
    static const float BLUR_SPREAD = 20.0f;

    static const uint32_t EDGE_BLUR_DEPTH = 5;
    static const float EDGE_BLUR_SPREAD = 20.0f;
    static const float EDGE_DEPTH_THRESHOLD = 10.0f;

    static const float VIBRANCE = 0.7f;

    WatercolorSubPipeline::WatercolorSubPipeline()
    {
        processingBuffers = std::make_unique<Rendering::WatercolorProcessingBuffers>();

        waterColorLowerResBlitPass = std::make_unique<Rendering::BlitMainColorPass>(
            Rendering::WC_COLORBLIT_TEX_A,
            Rendering::WC_BLIT_RESOLUTION,
            Rendering::RenderPassEvent::WATERCOLOR,
            0);

        blurHorizontalPass = std::make_unique<Rendering::GaussianBlurPass>(
            Rendering::WC_COLORBLIT_TEX_A,
            Rendering::WC_COLORBLIT_TEX_B,
            Rendering::WC_BLIT_RESOLUTION,
            Rendering::GaussDirection::Horizontal,
            BLUR_DEPTH,
            BLUR_SPREAD,
            Rendering::RenderPassEvent::WATERCOLOR,
            1);

        blurVerticalPass = std::make_unique<Rendering::GaussianBlurPass>(
            Rendering::WC_COLORBLIT_TEX_B,
            Rendering::WC_COLORBLIT_TEX_A,
            Rendering::WC_BLIT_RESOLUTION,
            Rendering::GaussDirection::Vertical,
            BLUR_DEPTH,
            BLUR_SPREAD,
            Rendering::RenderPassEvent::WATERCOLOR,
            2);

        edgeBlurHorizontalPass = std::make_unique<Rendering::OffsetEdgeBlurPass>(
            Rendering::GaussDirection::Horizontal,
            EDGE_BLUR_DEPTH,
            EDGE_BLUR_SPREAD,
            EDGE_DEPTH_THRESHOLD,
            Rendering::RenderPassEvent::WATERCOLOR,
            3);

        edgeBlurVerticalPass = std::make_unique<Rendering::OffsetEdgeBlurPass>(
            Rendering::GaussDirection::Vertical,
            EDGE_BLUR_DEPTH,
            EDGE_BLUR_SPREAD,
            EDGE_DEPTH_THRESHOLD,
            Rendering::RenderPassEvent::WATERCOLOR,
            4);

        watercolorPostProcessingPass = std::make_unique<Rendering::WatercolorPostProcessingPass>(
            processingBuffers.get());

        std::shared_ptr<Rendering::Material> filteringMat = std::make_shared<Rendering::Material>("Blit/WatercolorFiltering");
        filteringMat->SetFloat("_Vibrance", VIBRANCE);

        watercolorFilteringPass = std::make_unique<Rendering::FilterPostProcessingPass>(filteringMat,
            Rendering::RenderPassEvent::WATERCOLOR,
            7);
    }

    std::vector<Rendering::IRenderPass*> WatercolorSubPipeline::GetRenderPasses()
    {
        return {waterColorLowerResBlitPass.get(),
            blurHorizontalPass.get(),
            blurVerticalPass.get(),
            edgeBlurHorizontalPass.get(),
            edgeBlurVerticalPass.get(),
            watercolorPostProcessingPass.get(),
            watercolorFilteringPass.get()};
    }

} // namespace Beer::System
