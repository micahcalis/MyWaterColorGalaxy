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

    void WatercolorProcessingBuffers::ReallocateBlurredColor(const RenderContext& context)
    {
        uint32_t resolutionX = (uint32_t)((float)Core::Screen::Width() / (float)WC_BLIT_RESOLUTION);
        uint32_t resolutionY = (uint32_t)((float)Core::Screen::Height() / (float)WC_BLIT_RESOLUTION);

        BlurredColor = context.BlackBox->ReallocateIfNeeded(WC_COLORBLIT_TEX_A,
                                           resolutionX,
                                           resolutionY,
                                           Core::Screen::ColorFormat())
                           .AllocPointer;
    }

} // namespace Beer::Rendering
