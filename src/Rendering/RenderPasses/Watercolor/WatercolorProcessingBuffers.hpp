#pragma once

#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::Rendering
{
    class WatercolorProcessingBuffers
    {
    public:
        Rendering::RenderTexture* GBufferNormalsOffset = nullptr;
        Rendering::RenderTexture* GBufferWatercolor = nullptr;

    public:
        void ReallocateNormalsOffset(const RenderContext& context);
        void ReallocateWatercolor(const RenderContext& context);
    };
} // namespace Beer::Rendering
