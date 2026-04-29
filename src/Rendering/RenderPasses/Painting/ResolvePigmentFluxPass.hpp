#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "WaterColorSimBuffers.hpp"
namespace Beer::Rendering
{
    class ResolvePigmentFluxPass : public IRenderPass
    {
        WaterColorSimBuffers* simulationBuffers;

    public:
        ResolvePigmentFluxPass(WaterColorSimBuffers* simulationBuffers);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
