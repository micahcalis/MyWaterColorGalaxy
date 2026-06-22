#pragma once

#include "NebulaBuffer.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"

namespace Beer::Rendering
{
    static const float NEBULA_DEPTH_TOLERANCE = 0.005f;

    class BlendNebulaPass : public IRenderPass
    {
    private:
        NebulaBuffer* nebulaBuffer = nullptr;
        System::GalaxyObjectBuffer* stardustBuffer = nullptr;
        std::shared_ptr<Rendering::Material> blendMaterial = nullptr;

    public:
        BlendNebulaPass(NebulaBuffer* nebulaBuffer,
            System::GalaxyObjectBuffer* stardustBuffer);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

        bool BlitsMainTarget() const override
        {
            return true;
        }
    };
} // namespace Beer::Rendering
