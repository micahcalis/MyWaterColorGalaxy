#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/RenderPasses/Watercolor/NebulaBuffer.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"

namespace Beer::Rendering
{
    class TraceNebulaPass : public IRenderPass
    {
    private:
        NebulaBuffer* nebulaBuffer = nullptr;
        System::GalaxyObjectBuffer* stardustBuffer = nullptr;
        std::shared_ptr<Material> traceMaterial = nullptr;
        std::shared_ptr<Texture3D> shapeNoise = nullptr;
        std::shared_ptr<ComputeContext> shapeContext = nullptr;
        std::shared_ptr<Texture3D> detailNoise = nullptr;
        std::shared_ptr<ComputeContext> detailContext = nullptr;
        std::shared_ptr<Texture3D> distortionNoise = nullptr;
        std::shared_ptr<ComputeContext> distortionContextBase = nullptr;
        std::shared_ptr<ComputeContext> distortionContextCurl = nullptr;

    public:
        TraceNebulaPass(NebulaBuffer* nebulaBuffer,
            System::GalaxyObjectBuffer* stardustBuffer);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

    private:
        void InitializeNoiseVolumes();
    };
} // namespace Beer::Rendering
