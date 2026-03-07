#pragma once

#include "System/Camera/Camera.hpp"
#include "System/Galaxy/Player/PlayerHandle.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include <memory>
namespace Beer::System
{
    class PlayerCamera
    {
        const glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);

    private:
        const PlayerHandle player;
        std::unique_ptr<Camera> camera = nullptr;
        float yaw = -90.0f;
        float pitch = 0.0f;

    public:
        PlayerCamera(PlayerHandle player);
        void Update(PlayerInput input);

    private:
        void FollowPlayer();
        void RotateCamera(glm::vec2 mouseVec);
    };
} // namespace Beer::System
