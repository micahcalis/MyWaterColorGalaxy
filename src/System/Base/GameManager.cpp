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
#include "System/Galaxy/Player/PlayerInput.hpp"
#include "System/Galaxy/UI/GalaxyUserIntContext.hpp"
#include "System/Light/ILight.hpp"
#include "System/Light/LightManager.hpp"
#include "System/Menus/Background/GalaxyBackgroundContext.hpp"
#include "System/Menus/UI/TitleUserIntContext.hpp"
#include "System/PaintTool/PaintToolContext.hpp"
#include "System/Serialization/MapHandler.hpp"
#include "System/Serialization/MapSerializationManager.hpp"
#include <memory>

static const std::string TEST_MAP = "TestMap";

namespace Beer::System
{
    GameManager::~GameManager()
    {
        Camera::SetCameraManager(nullptr);
        ILight::SetLightManager(nullptr);
    }

    GameManager::GameManager(Function<void> quitApplication)
        : quitApplication(quitApplication)
    {
    }

    void GameManager::Initialize()
    {
        InitializeBase();
        InitializeSerialization();
        InitializeContext();
        InitializeContextFactory();
        InitializeColliders();

        // InitializePaintTool();
        InitializeMainMenu();
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
        Function<PlayerInput> getPlayerInput = [this]() -> PlayerInput { return GetPlayerInput(); };
        Function<MouseInput> getMouseInput = [this]() -> MouseInput { return inputManager->GetMouseInput(); };
        Function<ButtonInput> getDebugKeyInput = [this]() -> ButtonInput { return inputManager->GetDebugButtonInput(); };
        Function<bool> getReturnPressed = [this]() -> bool { return inputManager->GetTabButtonInput().ButtonExit; };
        Function<ButtonInput> getTabKeyInput = [this]() -> ButtonInput { return inputManager->GetTabButtonInput(); };

        contextHandler->RegisterContextFactory(ContextType::Galaxy,
            [this, getPlayerInput, getReturnPressed]() -> std::shared_ptr<IContext> {
                MapHandler handler = mapSerializationManager->GetMapHandler(TEST_MAP);
                return std::make_shared<GalaxyContext>(getPlayerInput, getReturnPressed, handler);
            });

        contextHandler->RegisterContextFactory(ContextType::GalaxyUserInt,
            [this]() -> std::shared_ptr<IContext> {
                return std::make_shared<GalaxyUserIntContext>();
            });

        contextHandler->RegisterContextFactory(ContextType::PaintTool,
            [this, getMouseInput, getDebugKeyInput, getTabKeyInput]() -> std::shared_ptr<PaintToolContext> {
                MapHandler handler = mapSerializationManager->GetMapHandler(TEST_MAP);
                return std::make_shared<PaintToolContext>(getMouseInput, getDebugKeyInput, getTabKeyInput, handler);
            });

        contextHandler->RegisterContextFactory(ContextType::GalaxyBackground,
            [this]() -> std::shared_ptr<GalaxyBackgroundContext> {
                return std::make_shared<GalaxyBackgroundContext>();
            });

        contextHandler->RegisterContextFactory(ContextType::MainMenu,
            [this]() -> std::shared_ptr<TitleUserIntContext> {
                return std::make_shared<TitleUserIntContext>();
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
        GalaxyContext* galaxyContext = contextHandler->GetContext<GalaxyContext>(ContextType::Galaxy);

        contextHandler->LoadContext(ContextType::GalaxyUserInt);
        GalaxyUserIntContext* uiContext = contextHandler->GetContext<GalaxyUserIntContext>(ContextType::GalaxyUserInt);

        galaxyContext->OnSetPhotoMode.Subscribe([uiContext](bool enabled) -> void { uiContext->SetDisplaysEnabled(!enabled); });

        Function<void> toPaintTool = [this]() -> void {
            contextHandler->DestroyContext(ContextType::Galaxy);
            contextHandler->DestroyContext(ContextType::GalaxyUserInt);
            InitializePaintTool();
        };

        Function<void> onReturnToPainting = [this, toPaintTool]() -> void {
            contextHandler->QueueOperation(toPaintTool);
        };

        galaxyContext->OnReturnToPainting.Subscribe(onReturnToPainting);
    }

    void GameManager::InitializePaintTool()
    {
        contextHandler->LoadContext(ContextType::PaintTool);

        PaintToolContext* context = contextHandler->GetContext<PaintToolContext>(ContextType::PaintTool);

        Function<void> toGalaxy = [this]() -> void {
            contextHandler->DestroyContext(ContextType::PaintTool);
            InitializeGalaxy();
        };

        Function<void> onGalaxyFly = [this, toGalaxy]() -> void {
            contextHandler->QueueOperation(toGalaxy);
        };

        context->OnGalaxyFly.Subscribe(onGalaxyFly);
    }

    void GameManager::InitializeMainMenu()
    {
        if (!contextHandler->ContextExists(ContextType::GalaxyBackground))
        {
            contextHandler->LoadContext(ContextType::GalaxyBackground);
        }

        contextHandler->LoadContext(ContextType::MainMenu);

        TitleUserIntContext* userIntContext = contextHandler->GetContext<TitleUserIntContext>(ContextType::MainMenu);
        userIntContext->OnCloseApplication.Subscribe(quitApplication);
    }

    void GameManager::UpdateBase()
    {
        clockManager->Update();
        inputManager->Update();
        cameraManager->Update();
        lightManager->Update();
    }

    PlayerInput GameManager::GetPlayerInput()
    {
        PlayerInput input{};
        input.MovementVec = inputManager->GetMovementVector();
        input.MouseVec = inputManager->GetMouseVector();
        input.IsBoosting = inputManager->GetSpaceButtonInput().ButtonHold;
        input.PhotoTogglePressed = inputManager->GetPButtonInput().ButtonExit;
        return input;
    }
} // namespace Beer::System
