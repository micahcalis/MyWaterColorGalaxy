#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include <cstdint>

namespace Beer::Rendering
{
    enum class FullscreenResolution : int32_t
    {
        Full = 1,
        Half = 2,
        Quarter = 4
    };

    class BlitMainColorPass : public IRenderPass
    {
    private:
        std::string destinationName;
        FullscreenResolution destinationResolution = FullscreenResolution::Full;
        std::shared_ptr<Rendering::Material> blitMaterial = nullptr;

    public:
        BlitMainColorPass(const std::string& destinationName,
            FullscreenResolution destinationResolution,
            RenderPassEvent event,
            int32_t eventOffset = 0);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
