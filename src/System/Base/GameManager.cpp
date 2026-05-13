#include "System/Base/GameManager.hpp"
#include "Input/ButtonInput.hpp"
#include "Input/MouseInput.hpp"
#include "System/Base/Clock/ClockManager.hpp"
#include "System/Base/Input/InputManager.hpp"
#include "System/Components/Colliders/QuadColliderManager.hpp"
#include "System/Context/ContextHandler.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Context/IContext.hpp"
#include "System/Context/WorldContainer.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Galaxy/GalaxyContext.hpp"
#include "System/Light/ILight.hpp"
#include "System/Light/LightManager.hpp"
#include "System/PaintTool/PaintToolContext.hpp"
#include "System/Serialization/MapHandler.hpp"
#include "System/Serialization/MapSerializationManager.hpp"
#include <memory>

static const std::string TEST_MAP = "TestMap";

namespace Beer::System
{
    void GameManager::Initialize()
    {
        InitializeBase();
        InitializeSerialization();
        InitializeContext();
        InitializeContextFactory();
        InitializeColliders();
        // temporary, we dont start gaming immediately
        InitializePaintTool();
    }

    void GameManager::PreUpdate()
    {
        OnMouseScrolled(0);
    }

    void GameManager::Update()
    {
        UpdateBase();
        contextHandler->Update();
        quadColliderManager->Update();
        worldContainer->UpdateContexts();
    }

    void GameManager::OnMouseScrolled(float scrollY)
    {
        if (inputManager != nullptr)
        {
            inputManager->SetScroll(scrollY);
        }
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

        Function<MouseInput> getMouseInput = [this]() -> MouseInput { return inputManager->GetMouseInput(); };
        Function<ButtonInput> getDebugKeyInput = [this]() -> ButtonInput { return inputManager->GetDebugButtonInput(); };

        contextHandler->RegisterContextFactory(ContextType::Galaxy,
            [this, getPlayerInput]() -> std::shared_ptr<IContext> {
                MapHandler handler = mapSerializationManager->GetMapHandler(TEST_MAP);
                return std::make_shared<GalaxyContext>(getPlayerInput, handler);
            });

        contextHandler->RegisterContextFactory(ContextType::PaintTool,
            [this, getMouseInput, getDebugKeyInput]() -> std::shared_ptr<PaintToolContext> {
                MapHandler handler = mapSerializationManager->GetMapHandler(TEST_MAP);
                return std::make_shared<PaintToolContext>(getMouseInput, getDebugKeyInput, handler);
            });
    }

    void GameManager::InitializeContext()
    {
        worldContainer = std::make_unique<WorldContainer>();
        contextHandler = std::make_unique<ContextHandler>(worldContainer.get());
        IContext::SetWorldContainer(worldContainer.get());
    }

    void GameManager::InitializeColliders()
    {
        quadColliderManager = std::make_unique<QuadColliderManager>([this]() -> MouseInput {
            return inputManager->GetMouseInput();
        });
    }

    void GameManager::InitializeSerialization()
    {
        mapSerializationManager = std::make_unique<MapSerializationManager>();
    }

    void GameManager::InitializeGalaxy()
    {
        contextHandler->LoadContext(ContextType::Galaxy);
    }

    void GameManager::InitializePaintTool()
    {
        contextHandler->LoadContext(ContextType::PaintTool);

        PaintToolContext* context = contextHandler->GetContext<PaintToolContext>(ContextType::PaintTool);

        Function<void> toGalaxy = [this]() -> void {
            contextHandler->DestroyContext(ContextType::PaintTool);
            contextHandler->LoadContext(ContextType::Galaxy);
        };

        Function<void> onGalaxyFly = [this, toGalaxy]() -> void {
            contextHandler->QueueOperation(toGalaxy);
        };

        context->OnGalaxyFly.Subscribe(onGalaxyFly);
    }

    void GameManager::UpdateBase()
    {
        clockManager->Update();
        inputManager->Update();
        cameraManager->Update();
        lightManager->Update();
    }
} // namespace Beer::System
