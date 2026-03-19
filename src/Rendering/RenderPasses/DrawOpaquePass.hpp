#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::Rendering
{
    class DrawOpaquePass : public IRenderPass
    {
    public:
        DrawOpaquePass()
            : IRenderPass("Opaque")
        {
        }

    private:
        RenderTexture* colorTarget = nullptr;

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
