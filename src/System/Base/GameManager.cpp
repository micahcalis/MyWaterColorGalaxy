#include "System/Base/GameManager.hpp"
#include "System/Base/Clock/ClockManager.hpp"
#include "System/Base/Input/InputManager.hpp"
#include "System/Context/ContextHandler.hpp"
#include "System/Context/IContext.hpp"
#include "System/Context/WorldContainer.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Galaxy/GalaxyContext.hpp"
#include "System/Light/ILight.hpp"
#include "System/Light/LightManager.hpp"
#include <memory>

namespace Beer::System
{
    void GameManager::Initialize()
    {
        InitializeBase();
        InitializeContext();
        InitializeContextFactory();
        // temporary, we dont start gaming immediately
        InitializeGalaxy();
    }

    void GameManager::Update()
    {
        UpdateBase();
        worldContainer->UpdateContexts();
    }

    void GameManager::InitializeBase()
    {
        clockManager = std::make_unique<ClockManager>();
        inputManager = std::make_unique<InputManager>();

        cameraManager = std::make_unique<CameraManager>();
        Camera::SetCameraManager(cameraManager.get());

        lightManager = std::make_unique<LightManager>();
        ILight::SetLightManager(lightManager.get());
    }

    void GameManager::InitializeContextFactory()
    {
        InputManager* inputManagerP = inputManager.get();
        auto getPlayerInput =
            [inputManagerP]() -> PlayerInput { return PlayerInput(inputManagerP->GetMovementVector(), inputManagerP->GetMouseVector()); };

        contextHandler->RegisterContextFactory(ContextType::Galaxy, [getPlayerInput]() -> std::shared_ptr<IContext> {
            return std::make_shared<GalaxyContext>(getPlayerInput);
        });
    }

    void GameManager::InitializeContext()
    {
        worldContainer = std::make_unique<WorldContainer>();
        contextHandler = std::make_unique<ContextHandler>(worldContainer.get());
        IContext::SetWorldContainer(worldContainer.get());
    }

    void GameManager::InitializeGalaxy()
    {
        contextHandler->LoadContext(ContextType::Galaxy);
    }

    void GameManager::UpdateBase()
    {
        clockManager->Update();
        cameraManager->Update();
        lightManager->Update();
    }
} // namespace Beer::System
