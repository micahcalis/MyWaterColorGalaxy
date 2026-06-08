#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Light/PointLight.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <memory>

namespace Beer::System
{
    class SunEntity : public GameEntity
    {
    private:
        std::shared_ptr<Rendering::Material> sunMaterial = nullptr;
        std::shared_ptr<Rendering::Mesh> sunMesh = nullptr;
        std::unique_ptr<PointLight> pointLight = nullptr;
        std::shared_ptr<Rendering::Texture3D> controlNoiseVolume = nullptr;

        std::shared_ptr<Rendering::ComputeContext> sunNoiseContext = nullptr;
        std::shared_ptr<Rendering::Texture3D> sunNoiseVolume = nullptr;

    public:
        SunEntity(std::shared_ptr<Rendering::Texture3D> controlNoiseVolume)
            : controlNoiseVolume(controlNoiseVolume)
            , GameEntity()
        {
        }

        void Update() override
        {
        }

        void LoadFromSerialized(const SerializableGalaxy& serializedGalaxy);

    private:
        void InitializeManager() override
        {
        }

        void InitializeSunNoise();
    };
} // namespace Beer::System
