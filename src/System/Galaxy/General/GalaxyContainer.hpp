#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"
#include "GalaxyObjectType.hpp"
#include <memory>
#include <unordered_map>

namespace Beer::System
{
    struct GalaxyBufferDefinition
    {
    public:
        const GalaxyObjectType Type;
        const char* ShaderPath;
        const char* MeshPath;

    public:
        GalaxyBufferDefinition(const GalaxyObjectType type,
            const char* shaderPath,
            const char* meshPath)
            : Type(type), ShaderPath(shaderPath), MeshPath(meshPath)
        {
        }
    };

    class GalaxyContainer
    {
    private:
        std::unordered_map<GalaxyObjectType, std::unique_ptr<GalaxyObjectBuffer>> bufferMap;
        // std::shared_ptr<Rendering::ComputeContext> turbulenceContext = nullptr;
        // std::shared_ptr<Rendering::Texture3D> turbulenceVolume = nullptr;
        // std::shared_ptr<Rendering::ComputeContext> tremorNoiseContext = nullptr;
        // std::shared_ptr<Rendering::Texture3D> tremorNoiseVolume = nullptr;
        std::shared_ptr<Rendering::ComputeContext> controlNoiseContext = nullptr;
        std::shared_ptr<Rendering::Texture3D> controlNoiseVolume = nullptr;

    public:
        void CreateBuffers(const SerializableGalaxy& serializedData);
        void Update();

        void Draw(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            const Rendering::ShaderPassType pass);

        void Clear()
        {
            bufferMap.clear();
        }

    private:
        void CreateNoiseVolumes();
    };
} // namespace Beer::System
