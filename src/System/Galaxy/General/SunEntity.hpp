#pragma once

#include "System/Components/Registry/GameEntity.hpp"
#include "System/Light/PointLight.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"

namespace Beer::System
{
    class SunEntity : public GameEntity
    {
    private:
        std::shared_ptr<Rendering::Material> sunMaterial = nullptr;
        std::shared_ptr<Rendering::Mesh> sunMesh = nullptr;
        std::unique_ptr<PointLight> pointLight = nullptr;

    public:
        SunEntity()
            : GameEntity()
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
    };
} // namespace Beer::System
