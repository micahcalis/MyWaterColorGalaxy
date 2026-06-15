#pragma once

#include "Rendering/RenderPasses/Watercolor/BlitMainColorPass.hpp"
#include "Rendering/RenderPasses/Watercolor/GaussianBlurPass.hpp"
#include "Rendering/RenderPasses/Watercolor/OffsetEdgeBlurPass.hpp"
#include "System/Components/General/ISubRenderPipeline.hpp"
#include <memory>
#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"

namespace Beer::System
{
    class WatercolorSubPipeline : public ISubRenderPipeline
    {
    private:
        std::unique_ptr<Rendering::WatercolorProcessingBuffers> processingBuffers = nullptr;
        std::unique_ptr<Rendering::BlitMainColorPass> waterColorLowerResBlitPass = nullptr;
        std::unique_ptr<Rendering::GaussianBlurPass> blurHorizontalPass = nullptr;
        std::unique_ptr<Rendering::GaussianBlurPass> blurVerticalPass = nullptr;
        std::unique_ptr<Rendering::OffsetEdgeBlurPass> edgeBlurHorizontalPass = nullptr;
        std::unique_ptr<Rendering::OffsetEdgeBlurPass> edgeBlurVerticalPass = nullptr;
        std::unique_ptr<Rendering::WatercolorPostProcessingPass> watercolorPostProcessingPass = nullptr;

    public:
        WatercolorSubPipeline();
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;
    };
} // namespace Beer::System
