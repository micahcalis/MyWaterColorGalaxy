#pragma once

#include "NebulaBuffer.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"

namespace Beer::Rendering
{
    class BlendNebulaWCPass : public IRenderPass
    {
    private:
        NebulaBuffer* nebulaBuffer = nullptr;
        System::GalaxyObjectBuffer* stardustBuffer = nullptr;
        std::shared_ptr<Rendering::Material> blendMaterial = nullptr;

    public:
        BlendNebulaWCPass(NebulaBuffer* nebulaBuffer,
            System::GalaxyObjectBuffer* stardustBuffer);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
