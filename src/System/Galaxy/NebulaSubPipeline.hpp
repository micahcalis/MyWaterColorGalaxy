#pragma once

#include "Rendering/RenderPasses/Watercolor/BlendNebulaPass.hpp"
#include "Rendering/RenderPasses/Watercolor/ComputeNebulaTilesPass.hpp"
#include "Rendering/RenderPasses/Watercolor/NebulaBuffer.hpp"
#include "Rendering/RenderPasses/Watercolor/ComputeNebulaTilesPass.hpp"
#include "Rendering/RenderPasses/Watercolor/NebulaBuffer.hpp"
#include "Rendering/RenderPasses/Watercolor/TraceNebulaPass.hpp"
#include "System/Components/General/ISubRenderPipeline.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"

namespace Beer::System
{
    class NebulaSubPipeline : public ISubRenderPipeline
    {
    private:
        GalaxyObjectBuffer* stardustBuffer = nullptr;
        std::unique_ptr<Rendering::NebulaBuffer> nebulaBuffer = nullptr;
        std::unique_ptr<Rendering::ComputeNebulaTilesPass> computeNebulaTilesPass = nullptr;
        std::unique_ptr<Rendering::TraceNebulaPass> traceNebulaPass = nullptr;
        std::unique_ptr<Rendering::BlendNebulaPass> blendNebulaPass = nullptr;

    public:
        NebulaSubPipeline(GalaxyObjectBuffer* stardustBuffer)
            : stardustBuffer(stardustBuffer)
        {
            nebulaBuffer = std::make_unique<Rendering::NebulaBuffer>();

            computeNebulaTilesPass = std::make_unique<Rendering::ComputeNebulaTilesPass>(nebulaBuffer.get(),
                stardustBuffer);

            traceNebulaPass = std::make_unique<Rendering::TraceNebulaPass>(nebulaBuffer.get(),
                stardustBuffer);

            blendNebulaPass = std::make_unique<Rendering::BlendNebulaPass>(nebulaBuffer.get(),
                stardustBuffer);
        }

        std::vector<Rendering::IRenderPass*> GetRenderPasses()
        {
            return {computeNebulaTilesPass.get(),
                traceNebulaPass.get(),
                blendNebulaPass.get()};
        };
    };
} // namespace Beer::System
