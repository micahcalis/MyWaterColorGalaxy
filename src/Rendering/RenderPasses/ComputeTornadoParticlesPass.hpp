#pragma once

#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"

namespace Beer::Rendering
{
    static uint32_t TORNADO_PART_COUNT = 1052;
    static const std::string TORNADO_BUFFER_NAME = "TornadoBuffer";

    class ComputeTornadoParticlesPass : public IRenderPass
    {
    private:
        std::shared_ptr<ComputeContext> instanceTestCompContext;
        PhaseBuffer* particleBuffer;

    public:
        ComputeTornadoParticlesPass(const uint32_t intEvent);

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
