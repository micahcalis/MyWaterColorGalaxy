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
        uint32_t totalTilesX = (Core::Screen::Width() / 2) / NEBULA_TILE_RES;
        uint32_t totalTilesY = (Core::Screen::Height() / 2) / NEBULA_TILE_RES;

        NebulaTilesBuffer = static_cast<PhaseBuffer*>(
            context.BlackBox->ReallocateIfNeeded(NEBULA_TILES_NAME,
                                sizeof(NebulaTile) * totalTilesX * totalTilesY)
                .AllocPointer);
    }

    void NebulaBuffer::ReallocateTarget(const RenderContext& context)
    {
        glm::uvec2 tileCount = GetDispatchTileCount();

        NebulaMarchingTarget = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(NEBULA_TARGET_NAME,
                                (uint32_t)tileCount.x,
                                (uint32_t)tileCount.y,
                                static_cast<VkFormat>(NEBULA_TARGET_FORMAT))
                .AllocPointer);
    }
} // namespace Beer::Rendering
