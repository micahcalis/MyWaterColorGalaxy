#pragma once

#include "Buffer/AsteroidsBufferSettings.hpp"
#include "Buffer/BlackHoleBufferSettings.hpp"
#include "System/Galaxy/General/Buffer/GalaxyBufferSettings.hpp"
#include "System/Galaxy/General/Buffer/PlanetBufferSettings.hpp"
#include "System/Galaxy/General/Buffer/SpaceGooBufferSettings.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"
#include "GalaxyObjectType.hpp"
#include <array>
#include <memory>
#include <unordered_map>

namespace Beer::System
{
    struct GalaxyBufferDefinition
    {
    public:
        const GalaxyObjectType Type;
        const char* ShaderPath;
        std::shared_ptr<IGalaxyBufferSettings> Settings = nullptr;

    public:
        GalaxyBufferDefinition(const GalaxyObjectType type,
            const char* shaderPath,
            const std::shared_ptr<IGalaxyBufferSettings> settings)
            : Type(type), ShaderPath(shaderPath), Settings(settings)
        {
        }
    };

    class GalaxyContainer
    {
    private:
        std::unordered_map<GalaxyObjectType, std::unique_ptr<GalaxyObjectBuffer>> bufferMap;
        std::shared_ptr<Rendering::ComputeContext> controlNoiseContext = nullptr;
        std::shared_ptr<Rendering::Texture3D> controlNoiseVolume = nullptr;
        std::array<GalaxyBufferDefinition, 5> definitions;

    public:
        GalaxyContainer()
            : definitions({GalaxyBufferDefinition(GalaxyObjectType::Planet,
                               "Galaxy/Planet",
                               std::make_shared<PlanetBufferSettings>()),
                  GalaxyBufferDefinition(GalaxyObjectType::Asteroids,
                      "Galaxy/Asteroids",
                      std::make_shared<AsteroidsBufferSettings>()),
                  GalaxyBufferDefinition(GalaxyObjectType::SpaceGoo,
                      "Galaxy/SpaceGoo",
                      std::make_shared<SpaceGooBufferSettings>()),
                  GalaxyBufferDefinition(GalaxyObjectType::BlackHole,
                      "Galaxy/BlackHole",
                      std::make_shared<BlackHoleBuferSettings>()),
                  GalaxyBufferDefinition(GalaxyObjectType::StarDust,
                      "Galaxy/Planet",
                      std::make_shared<UnimplementedBufferSettings>(0.02f,
                          200.0f,
                          600.0f,
                          0.2f,
                          0.2f,
                          0.5f,
                          0.8f,
                          "MDL_Cube"))})
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
