#pragma once

#include "PlayerEntity.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include <memory>

namespace Beer::System
{
    class PlayerCamera
    {
        const glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);

    private:
        PlayerEntity* player;
        GameSubEntity* cameraEntity;
        std::unique_ptr<Camera> camera = nullptr;
        float yaw = -90.0f;
        float pitch = 0.0f;

    public:
        PlayerCamera(PlayerEntity* player, GameSubEntity* cameraEntity);
        void Update(PlayerInput input);

    private:
        void FollowPlayer();
        void RotateCamera(glm::vec2 mouseVec);
    };
} // namespace Beer::System
