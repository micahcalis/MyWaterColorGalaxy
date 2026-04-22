#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "WaterColorSimBuffers.hpp"

namespace Beer::Rendering
{
    static const std::string CANVAS_BASE_NAME = "ColorMixing/Tex_Mixing_Canvas";

    class GenerateCanvasPass : public IRenderPass
    {
    private:
        WaterColorSimBuffers* simulationBuffers;
        std::shared_ptr<Texture2D> canvasBaseTexture;

    public:
        GenerateCanvasPass(WaterColorSimBuffers* simulationBuffers);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
