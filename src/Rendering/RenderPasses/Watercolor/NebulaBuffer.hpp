#pragma once

#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/RenderPasses/Watercolor/BlitMainColorPass.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::Rendering
{
    static const FullscreenResolution NEBULA_RESOLUTION = FullscreenResolution::Half;
    static const vk::Format NEBULA_TARGET_FORMAT = vk::Format::eR8G8B8A8Unorm;
    static const std::string NEBULA_TARGET_NAME = "_NebulaTarget";

    static const uint32_t NEBULA_TILE_RES = 16;
    static const uint32_t MAX_NEBULA_PER_TILE = 32;
    static const std::string NEBULA_TILES_NAME = "_NebulaTiles";

    struct NebulaTile
    {
        uint32_t Count;
        std::array<uint32_t, MAX_NEBULA_PER_TILE> Indices;
    };

    class NebulaBuffer
    {
    public:
        PhaseBuffer* NebulaTilesBuffer = nullptr;
        RenderTexture* NebulaMarchingTarget = nullptr;
        std::shared_ptr<ComputeContext> ComputeTilesContext = nullptr;

    public:
        NebulaBuffer();
        void ReallocateTiles(const RenderContext& context);
        void ReallocateTarget(const RenderContext& context);

        static glm::vec2 GetExactTileResolution()
        {
            float halfWidth = (float)Core::Screen::Width() / static_cast<float>(NEBULA_RESOLUTION);
            float halfHeight = (float)Core::Screen::Height() / static_cast<float>(NEBULA_RESOLUTION);
            return glm::vec2(halfWidth / NEBULA_TILE_RES, halfHeight / NEBULA_TILE_RES);
        }

        static glm::uvec2 GetDispatchTileCount()
        {
            glm::vec2 exact = GetExactTileResolution();
            return glm::uvec2((uint32_t)glm::ceil(exact.x), (uint32_t)glm::ceil(exact.y));
        }
    };
} // namespace Beer::Rendering
