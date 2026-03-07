#pragma once

#include "PlayerCamera.hpp"
#include "System/Context/ContextType.hpp"
#include "System/ECS/ECS.hpp"
#include "System/ECS/Registry.hpp"
#include "System/Galaxy/Player/PlayerHandle.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"

namespace Beer::System
{
    class PlayerManager
    {
    private:
        PlayerHandle player;
        std::unique_ptr<PlayerController> playerController = nullptr;
        std::unique_ptr<PlayerCamera> playerCamera = nullptr;
        bool movementEnabled;

    public:
        PlayerManager(const ContextType playerContext);
        void Update(PlayerInput input);
        void SetMovementEnabled(const bool enabled);
    };
} // namespace Beer::System

