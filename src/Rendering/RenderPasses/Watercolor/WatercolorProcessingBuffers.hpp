#pragma once

#include "BlitMainColorPass.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::Rendering
{
    static const std::string WC_COLORBLIT_TEX_A = "WatercolorBlitTextureA";
    static const std::string WC_COLORBLIT_TEX_B = "WatercolorBlitTextureB";
    static const Rendering::FullscreenResolution WC_BLIT_RESOLUTION = Rendering::FullscreenResolution::Half;

    class WatercolorProcessingBuffers
    {
    public:
        Rendering::RenderTexture* GBufferNormalsOffset = nullptr;
        Rendering::RenderTexture* GBufferWatercolor = nullptr;
        Rendering::RenderTexture* BlurredColor = nullptr;

    public:
        void ReallocateNormalsOffset(const RenderContext& context);
        void ReallocateWatercolor(const RenderContext& context);
        void ReallocateBlurredColor(const RenderContext& context);
    };
} // namespace Beer::Rendering
