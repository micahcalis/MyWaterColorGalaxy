#pragma once

#include "BlitMainColorPass.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"

namespace Beer::Rendering
{
    enum class GaussDirection : int
    {
        Horizontal = 0,
        Vertical = 1
    };

    class GaussianBlurPass : public IRenderPass
    {
    private:
        std::string sourceName;
        std::string destinationName;
        FullscreenResolution destinationResolution;
        GaussDirection blurDirection = GaussDirection::Horizontal;
        uint32_t blurDepth = 1;
        float blurSpread = 1.0f;
        RenderTexture* sourceTexture = nullptr;
        std::shared_ptr<Rendering::Material> blurMaterial = nullptr;

    public:
        GaussianBlurPass(const std::string& sourceName,
            const std::string& destinationName,
            FullscreenResolution destinationResolution,
            GaussDirection blurDirection,
            uint32_t blurDepth,
            float blurSpread,
            RenderPassEvent event,
            uint32_t offset = 0);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
