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
        float speed = 0;

    public:
        PlayerController(PlayerEntity* player)
            : player(player)
        {
        }

        void Update(PlayerInput playerInput);
        void Move(PlayerInput playerInput);
        float GetNormalizedSpeed() const;

    private:
        void HandleSpeed(bool isBoosting);
    };
} // namespace Beer::System
