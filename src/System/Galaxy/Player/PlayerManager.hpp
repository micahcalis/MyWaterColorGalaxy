#pragma once

#include "PlayerCamera.hpp"
#include "PlayerEntity.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"

namespace Beer::System
{
    class PlayerManager : public IEntityManager
    {
    private:
        PlayerEntity* player;
        std::unique_ptr<PlayerController> playerController = nullptr;
        std::unique_ptr<GameSubEntity> cameraEntity = nullptr;
        std::unique_ptr<PlayerCamera> playerCamera = nullptr;
        bool movementEnabled;
        Function<PlayerInput> getPlayerInput;

    public:
        PlayerManager(PlayerEntity* player, Function<PlayerInput> getPlayerInput);
        void Update() override;
        void SetMovementEnabled(const bool enabled);
    };
} // namespace Beer::System

