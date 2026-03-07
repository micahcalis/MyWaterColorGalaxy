#pragma once

#include "PlayerInput.hpp"
#include "System/Galaxy/Player/PlayerHandle.hpp"

namespace Beer::System
{
    const glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);

    class PlayerController
    {
    private:
        const PlayerHandle player;
        float yaw = -90.0f;
        float pitch = 0.0f;

    public:
        PlayerController(const PlayerHandle player)
            : player(player)
        {
        }

        void Update(PlayerInput playerInput);
        void Move(glm::vec2 movementVec);
        void Rotate(glm::vec2 mouseVec);
    };
} // namespace Beer::System
