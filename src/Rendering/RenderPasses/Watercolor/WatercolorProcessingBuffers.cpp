#include "Rendering/RenderPasses/Watercolor/WatercolorProcessingBuffers.hpp"

namespace Beer::Rendering
{
    void WatercolorProcessingBuffers::ReallocateNormalsOffset(const RenderContext& context)
    {
        GBufferNormalsOffset = context.BlackBox->ReallocateIfNeeded(std::string(GBUFFER_NORMAL_OFFSET),
                                                   Core::Screen::Width(),
                                                   Core::Screen::Height(),
                                                   GBUFFER_NORMAL_OFFSET_FORMAT)
                                   .AllocPointer;
    }

    void WatercolorProcessingBuffers::ReallocateWatercolor(const RenderContext& context)
    {
        GBufferWatercolor = context.BlackBox->ReallocateIfNeeded(GBUFFER_WATERCOLOR,
                                                Core::Screen::Width(),
                                                Core::Screen::Height(),
                                                GBUFFER_WATERCOLOR_FORMAT)
                                .AllocPointer;
    }

} // namespace Beer::Rendering
