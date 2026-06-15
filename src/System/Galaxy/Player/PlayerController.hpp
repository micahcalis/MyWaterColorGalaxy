#pragma once

#include "PlayerInput.hpp"

namespace Beer::System
{
    class PlayerEntity;

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
        static void SanitizeMouseInput(glm::vec2& mouseVec);

    private:
        void HandleSpeed(bool isBoosting);
    };
} // namespace Beer::System
