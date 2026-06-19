#include "Rendering/RenderPasses/Watercolor/NebulaBuffer.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "NebulaBuffer.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Buffer/SSBOType.hpp"
#include "Rendering/RenderPasses/Watercolor/BlitMainColorPass.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::Rendering
{
    NebulaBuffer::NebulaBuffer()
    {
        ComputeTilesContext = std::make_shared<ComputeContext>("Watercolor/NebulaTiles");
    }

    void NebulaBuffer::ReallocateTiles(const RenderContext& context)
    {
        glm::uvec2 tileCount = GetDispatchTileCount();

        NebulaTilesBuffer = static_cast<PhaseBuffer*>(
            context.BlackBox->ReallocateIfNeeded(NEBULA_TILES_NAME,
                                sizeof(NebulaTile) * tileCount.x * tileCount.y)
                .AllocPointer);
    }

    void NebulaBuffer::ReallocateTarget(const RenderContext& context)
    {
        uint32_t width = Core::Screen::Width() / static_cast<uint32_t>(NEBULA_RESOLUTION);
        uint32_t height = Core::Screen::Height() / static_cast<uint32_t>(NEBULA_RESOLUTION);

        NebulaMarchingTarget = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(NEBULA_TARGET_NAME,
                                width,
                                height,
                                static_cast<VkFormat>(NEBULA_TARGET_FORMAT))
                .AllocPointer);
    }

    void NebulaBuffer::ReallocateVolumeAlpha(const RenderContext& context)
    {
        NebulaVolumeAlpha = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(NEBULA_VOLUME_ALPHA_NAME,
                                Core::Screen::Width(),
                                Core::Screen::Height(),
                                static_cast<VkFormat>(NEBULA_VOLUME_ALPHA_FORMAT))
                .AllocPointer);
    }
} // namespace Beer::Rendering
