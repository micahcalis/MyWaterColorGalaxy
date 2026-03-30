#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"

namespace Beer::Rendering
{
    static uint32_t PERLIN_TEX_RES = 1052;
    static const std::string PERLIN_TEX_NAME = "PerlinComputeTex";

    class ComputePerlinPass : public IRenderPass
    {
    private:
        std::shared_ptr<ComputeContext> perlinCompContext;
        Material* perlinViewMat;

    public:
        ComputePerlinPass(const uint32_t intEvent, Material* perlinViewMat);

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
