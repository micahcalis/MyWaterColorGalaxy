#pragma once

#include "PlayerVFXManager.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/GameEntity.hpp"
namespace Beer::System
{
    class PlayerVFXEntity : public GameEntity
    {
    private:
        Function<Transform> getPlayerTransformData = nullptr;
        Function<bool> isBoosting = nullptr;
        std::shared_ptr<Rendering::Material> flameMaterial = nullptr;
        std::shared_ptr<Rendering::Mesh> flameMesh = nullptr;
        std::shared_ptr<Rendering::Texture3D> controlNoiseVolume = nullptr;

    public:
        PlayerVFXEntity(Function<Transform> getPlayerTransformData,
            Function<bool> isBoosting,
            std::shared_ptr<Rendering::Texture3D> controlNoiseVolume,
            Layer layer = Layer::Default);

        void Update() override
        {
            manager->Update();
        }

    private:
        void InitializeManager() override
        {
            manager = std::make_unique<PlayerVFXManager>(getPlayerTransformData,
                isBoosting,
                &transform,
                flameMaterial.get());
        }
    };
} // namespace Beer::System
