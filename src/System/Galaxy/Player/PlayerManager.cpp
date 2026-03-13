#include "System/Galaxy/Player/PlayerManager.hpp"
#include "PlayerEntity.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"

namespace Beer::System
{
    PlayerManager::PlayerManager(PlayerEntity* player, Function<PlayerInput> getPlayerInput)
        : getPlayerInput(getPlayerInput)
    {
        playerController = std::make_unique<PlayerController>(player);
        cameraEntity = std::make_unique<GameSubEntity>(Transform());
        playerCamera = std::make_unique<PlayerCamera>(player, cameraEntity.get());
        movementEnabled = true;
    }

    void PlayerManager::Update()
    {
        PlayerInput input = getPlayerInput();

        if (movementEnabled)
        {
            playerController->Update(input);
        }

        playerCamera->Update(input);
    }

    void PlayerManager::SetMovementEnabled(const bool enabled)
    {
        movementEnabled = enabled;
    }
} // namespace Beer::System
