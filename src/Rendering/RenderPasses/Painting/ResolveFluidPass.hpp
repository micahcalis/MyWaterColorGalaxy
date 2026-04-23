#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
namespace Beer::Rendering
{
    class ResolveFluidPass : public IRenderPass
    {
        WaterColorSimBuffers* simulationBuffers;

    public:
        ResolveFluidPass(WaterColorSimBuffers* simulationBuffers);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
