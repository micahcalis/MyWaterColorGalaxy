#pragma once

#include "System/Components/General/ISubRenderPipeline.hpp"
#include <memory>
#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"

namespace Beer::System
{
    class WatercolorSubPipeline : public ISubRenderPipeline
    {
    private:
        std::unique_ptr<Rendering::WatercolorProcessingBuffers> processingBuffers = nullptr;
        Rendering::WatercolorPostProcessingPass* watercolorPostProcessingPass = nullptr;

    public:
        WatercolorSubPipeline();
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;
    };
} // namespace Beer::System
