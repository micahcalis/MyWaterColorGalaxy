#pragma once

#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::Rendering
{
    class DrawUIPass : public IRenderPass
    {
    public:
        DrawUIPass()
            : IRenderPass("UserInterface", RenderPassEvent::USER_INTERFACE)
        {
        }

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
