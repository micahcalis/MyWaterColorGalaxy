#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
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

        std::shared_ptr<ComputeContext> noiseContext = nullptr;
        std::shared_ptr<Texture2D> noiseCubemap = nullptr;
        std::shared_ptr<ComputeContext> blurNoiseContextA = nullptr;
        std::shared_ptr<Texture2D> blurredNoiseCubemapA = nullptr;
        std::shared_ptr<ComputeContext> blurNoiseContextB = nullptr;
        std::shared_ptr<Texture2D> blurredNoiseCubemapB = nullptr;

    public:
        DrawSkyboxPass(std::shared_ptr<Rendering::Texture3D> controlNoiseVolume);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

    public:
        void InitializeNoiseCubemaps(const System::SerializableGalaxy& serializedGalaxy);
    };
} // namespace Beer::Rendering
