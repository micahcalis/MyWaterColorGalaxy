#pragma once

#include "Buffer/GalaxyBufferSettings.hpp"
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
        std::shared_ptr<IGalaxyBufferSettings> Settings = nullptr;

    public:
        GalaxyBufferDefinition(const GalaxyObjectType type,
            const char* shaderPath,
            const char* meshPath,
            const std::shared_ptr<IGalaxyBufferSettings> settings)
            : Type(type), ShaderPath(shaderPath), MeshPath(meshPath), Settings(settings)
        {
        }
    };

    class GalaxyContainer
    {
    private:
        std::unordered_map<GalaxyObjectType, std::unique_ptr<GalaxyObjectBuffer>> bufferMap;
        std::shared_ptr<Rendering::ComputeContext> controlNoiseContext = nullptr;
        std::shared_ptr<Rendering::Texture3D> controlNoiseVolume = nullptr;

    public:
        GalaxyContainer()
        {
            CreateNoiseVolumes();
        }

        void CreateBuffers(const SerializableGalaxy& serializedData);
        void Update();

        void Draw(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            const Rendering::ShaderPassType pass);

        void Clear()
        {
            bufferMap.clear();
        }

        std::shared_ptr<Rendering::Texture3D> GetControlNoiseVolume() const { return controlNoiseVolume; }

    private:
        void CreateNoiseVolumes();
    };
} // namespace Beer::System
