#pragma once

#include "NebulaBuffer.hpp"
#include "Rendering/RenderPasses/Watercolor/NebulaBuffer.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"

namespace Beer::Rendering
{
    class ComputeNebulaTilesPass : public IRenderPass
    {
    private:
        NebulaBuffer* nebulaBuffer = nullptr;
        System::GalaxyObjectBuffer* stardustBuffer = nullptr;

    public:
        ComputeNebulaTilesPass(NebulaBuffer* nebulaBuffer,
            System::GalaxyObjectBuffer* stardustBuffer);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
