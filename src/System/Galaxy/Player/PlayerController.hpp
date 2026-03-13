#pragma once

#include "PlayerEntity.hpp"
#include "PlayerInput.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"

namespace Beer::System
{
    class PlayerController
    {
    private:
        PlayerEntity* player;

    public:
        PlayerController(PlayerEntity* player)
            : player(player)
        {
        }

        void Update(PlayerInput playerInput);
        void Move(glm::vec2 movementVec);
    };
} // namespace Beer::System
