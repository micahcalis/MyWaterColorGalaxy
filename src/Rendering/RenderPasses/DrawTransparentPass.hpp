#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"

namespace Beer::Rendering
{
    class DrawTransparentPass : public IRenderPass
    {
    public:
        DrawTransparentPass()
            : IRenderPass("Transparent", RenderPassEvent::TRANSPARENT)
        {
        }

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
