#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Watercolor/GaussianBlurPass.hpp"

namespace Beer::Rendering
{
    class OffsetEdgeBlurPass : public IRenderPass
    {
    private:
        GaussDirection blurDirection = GaussDirection::Horizontal;
        uint32_t blurDepth = 1;
        float blurSpread = 1.0f;
        float depthTreshold = 10.0f;

        RenderTexture* gBufferNormalOffset = nullptr;
        RenderTexture* gBufferNormalOffsetPong = nullptr;
        std::shared_ptr<Rendering::Material> blurMaterial = nullptr;

    public:
        OffsetEdgeBlurPass(GaussDirection blurDirection,
            uint32_t blurDepth,
            float blurSpread,
            float depthThreshold,
            RenderPassEvent event,
            uint32_t offset);

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
