#pragma once

#include "PlayerInput.hpp"
#include "System/Galaxy/Player/PlayerHandle.hpp"

namespace Beer::System
{
    class PlayerController
    {
    private:
        PlayerHandle player;
        float yaw = -90.0f;
        float pitch = 0.0f;

    public:
        PlayerController(PlayerHandle player)
            : player(player)
        {
        }

        void Update(PlayerInput playerInput);
        void Move(glm::vec2 movementVec);
    };
} // namespace Beer::System
