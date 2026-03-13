#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Player/PlayerManager.hpp"
#include <memory>

namespace Beer::System
{
    void PlayerEntity::Update()
    {
        manager->Update();
    }

    void PlayerEntity::InitializeManager()
    {
        manager = std::make_unique<PlayerManager>(this, getPlayerInput);
    }

} // namespace Beer::System
