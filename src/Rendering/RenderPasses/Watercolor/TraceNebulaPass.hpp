#pragma once

#include "Rendering/RenderPasses/Watercolor/NebulaBuffer.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"

namespace Beer::Rendering
{
    class TraceNebulaPass : public IRenderPass
    {
    private:
        NebulaBuffer* nebulaBuffer = nullptr;
        System::GalaxyObjectBuffer* stardustBuffer = nullptr;
        std::shared_ptr<Rendering::Material> traceMaterial = nullptr;

    public:
        TraceNebulaPass(NebulaBuffer* nebulaBuffer,
            System::GalaxyObjectBuffer* stardustBuffer);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
