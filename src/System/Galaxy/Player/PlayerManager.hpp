#pragma once

#include "System/Context/ContextType.hpp"
#include "System/ECS/ECS.hpp"
#include "System/ECS/Registry.hpp"
#include "System/Galaxy/Player/Player.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"

namespace Beer::System
{
    class PlayerManager
    {
    private:
        Player player;
        std::unique_ptr<PlayerController> playerController = nullptr;
        bool movementEnabled;

    public:
        PlayerManager(const ContextType playerContext);
        void Update(PlayerInput input);
        void SetMovementEnabled(const bool enabled);
    };
} // namespace Beer::System

