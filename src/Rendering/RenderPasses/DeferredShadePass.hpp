#pragma once

#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include <memory>

namespace Beer::Rendering
{
    class DeferredShadePass : public IRenderPass
    {
    private:
        std::shared_ptr<Material> blitMaterial;
        RenderTexture* GBufferAlbedo;
        RenderTexture* GBufferNormal;
        RenderTexture* GBufferMaterial;
        RenderTexture* GBufferEmission;

    public:
        DeferredShadePass()
            : IRenderPass("Deferred Shade", RenderPassEvent::DEFERRED_SHADE)
        {
            blitMaterial = std::make_shared<Material>("DeferredShadeBlit");
        }

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
