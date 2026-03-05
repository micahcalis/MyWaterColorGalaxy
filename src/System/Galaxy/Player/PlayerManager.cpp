#include "System/Galaxy/Player/PlayerManager.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Context/IContext.hpp"
#include "System/ECS/Components/Transform.hpp"
#include "System/ECS/ECS.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"

namespace Beer::System
{
    PlayerManager::PlayerManager(const ContextType playerContext)
    {
        Registry& registry = IContext::GetRegistry(playerContext);
        Entity playerEntity = registry.Create();
        Transform playerTransform = registry.AddComponent<Transform>(playerEntity);
        player = Player(registry.GetHandle(playerEntity), &playerTransform);

        playerController = std::make_unique<PlayerController>(player);
        movementEnabled = true;
    }

    void PlayerManager::Update(PlayerInput input)
    {
        if (movementEnabled)
        {
            playerController->Update(input);
        }
    }

    void PlayerManager::SetMovementEnabled(const bool enabled)
    {
        movementEnabled = enabled;
    }
} // namespace Beer::System
