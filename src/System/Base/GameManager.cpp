#include "System/Base/GameManager.hpp"
#include "System/Base/Clock/ClockManager.hpp"
#include "System/Base/Input/InputManager.hpp"
#include "System/Context/ContextHandler.hpp"
#include "System/Context/IContext.hpp"
#include "System/Context/WorldContainer.hpp"
#include <memory>

namespace Beer::System
{
    void GameManager::Initialize()
    {
        InitializeBase();
        InitializeContext();
    }

    void GameManager::Update()
    {
        UpdateBase();
        worldContainer->UpdateContexts();
        playerManager->Update(GetPlayerInput());
    }

    void GameManager::InitializeBase()
    {
        clockManager = std::make_unique<ClockManager>();
        inputManager = std::make_unique<InputManager>();
    }

    void GameManager::InitializeContext()
    {
        worldContainer = std::make_unique<WorldContainer>();
        contextHandler = std::make_unique<ContextHandler>(worldContainer.get());
        IContext::SetWorldContainer(worldContainer.get());

        // this is temporary, we don't start gaming immediately
        InitializeGalaxy();
    }

    void GameManager::InitializeGalaxy()
    {
        contextHandler->LoadContext(ContextType::Galaxy);
        playerManager = std::make_unique<PlayerManager>(ContextType::Galaxy);
    }

    void GameManager::UpdateBase()
    {
        clockManager->Update();
    }

    PlayerInput GameManager::GetPlayerInput()
    {
        return {
            inputManager->GetMovementVector(),
            inputManager->GetMouseVector()};
    }
} // namespace Beer::System
