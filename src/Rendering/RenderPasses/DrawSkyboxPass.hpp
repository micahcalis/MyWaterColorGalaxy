#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Components/General/Transform.hpp"
#include <memory>

namespace Beer::Rendering
{
    class DrawSkyboxPass : public IRenderPass
    {
    private:
        std::shared_ptr<Material> skyboxMaterial = nullptr;
        std::shared_ptr<Mesh> cubeMesh = nullptr;
        System::Transform skyboxTransform{};
        std::shared_ptr<Rendering::Texture3D> controlNoiseVolume = nullptr;

    public:
        DrawSkyboxPass(std::shared_ptr<Rendering::Texture3D> controlNoiseVolume);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
    };
} // namespace Beer::Rendering
