#include "System/Base/GameManager.hpp"
#include "System/Base/Clock/ClockManager.hpp"

namespace Beer::System
{
    void GameManager::InitializeGame()
    {
        InitializeBase();
    }

    void GameManager::Update()
    {
        UpdateBase();
    }

    void GameManager::InitializeBase()
    {
        clockManager = std::make_unique<ClockManager>();
    }

    void GameManager::UpdateBase()
    {
        clockManager->Update();
    }
} // namespace Beer::System
