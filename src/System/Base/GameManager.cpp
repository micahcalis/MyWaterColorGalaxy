#include "System/Base/GameManager.hpp"
#include "Input/ButtonInput.hpp"
#include "Input/Input.hpp"
#include "Input/MouseInput.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "System/Base/Clock/ClockManager.hpp"
#include "System/Base/Input/ButtonInput.hpp"
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
#include "System/Menus/UI/SelectionUserIntContext.hpp"
#include "System/Menus/UI/TitleUserIntContext.hpp"
#include "System/PaintTool/PaintToolContext.hpp"
#include "System/Serialization/MapHandler.hpp"
#include "System/Serialization/MapSerializationManager.hpp"
#include <memory>

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
        inputManager = std::make_unique<InputManager>(InputMode::PenDisplay);

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
        Function<ButtonInput> getEscKeyInput = [this]() -> ButtonInput { return inputManager->GetEscButtonInput(); };

        contextHandler->RegisterContextFactory(ContextType::Galaxy,
            [this, getPlayerInput, getReturnPressed]() -> std::shared_ptr<IContext> {
                MapHandler handler = mapSerializationManager->GetMapHandler(currentMapName);
                return std::make_shared<GalaxyContext>(getPlayerInput, getReturnPressed, handler);
            });

        contextHandler->RegisterContextFactory(ContextType::GalaxyUserInt,
            [this]() -> std::shared_ptr<IContext> {
                return std::make_shared<GalaxyUserIntContext>();
            });

        contextHandler->RegisterContextFactory(ContextType::PaintTool,
            [this, getMouseInput, getDebugKeyInput, getTabKeyInput]() -> std::shared_ptr<PaintToolContext> {
                MapHandler handler = mapSerializationManager->GetMapHandler(currentMapName);
                return std::make_shared<PaintToolContext>(getMouseInput, getDebugKeyInput, getTabKeyInput, handler, fadeState);
            });

        contextHandler->RegisterContextFactory(ContextType::GalaxyBackground,
            [this]() -> std::shared_ptr<GalaxyBackgroundContext> {
                return std::make_shared<GalaxyBackgroundContext>();
            });

        contextHandler->RegisterContextFactory(ContextType::MainMenu,
            [this]() -> std::shared_ptr<TitleUserIntContext> {
                return std::make_shared<TitleUserIntContext>();
            });

        contextHandler->RegisterContextFactory(ContextType::SelectionMenu,
            [this, getEscKeyInput]() -> std::shared_ptr<SelectionUserIntContext> {
                return std::make_shared<SelectionUserIntContext>(getEscKeyInput);
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
            fadeState = Rendering::FadeState::In;
            contextHandler->QueueOperation(toPaintTool);
        };

        galaxyContext->OnReturnToPainting.Subscribe(onReturnToPainting);

        if (Input::Mode() == InputMode::PenDisplay)
        {
            Function<void> fadeReturn = [galaxyContext]() -> void {
                galaxyContext->FadeReturn();
            };

            uiContext->OnReturnClicked.Subscribe(fadeReturn);

            Function<void> togglePhotoMode = [galaxyContext]() -> void {
                galaxyContext->TogglePhotoMode();
            };

            uiContext->OnPhotoModeToggled.Subscribe(togglePhotoMode);
        }
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

        Function<void> toMainMenu = [this]() -> void {
            contextHandler->DestroyContext(ContextType::PaintTool);
            InitializeMainMenu();
        };

        Function<void> onBackToTitle = [this, toMainMenu]() -> void {
            contextHandler->QueueOperation(toMainMenu);
        };

        context->OnBackToTitle.Subscribe(onBackToTitle);
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

        Function<void> toSelectMenu = [this]() -> void {
            contextHandler->DestroyContext(ContextType::MainMenu);
            InitializeSelectMenu();
        };

        Function<void> onSelectMenu = [this, toSelectMenu] -> void {
            contextHandler->QueueOperation(toSelectMenu);
        };

        userIntContext->OnOpenSelection.Subscribe(onSelectMenu);
    }

    void GameManager::InitializeSelectMenu()
    {
        if (!contextHandler->ContextExists(ContextType::GalaxyBackground))
        {
            contextHandler->LoadContext(ContextType::GalaxyBackground);
        }

        contextHandler->LoadContext(ContextType::SelectionMenu);

        SelectionUserIntContext* userIntContext = contextHandler->GetContext<SelectionUserIntContext>(ContextType::SelectionMenu);

        Function<void> toMainMenu = [this]() -> void {
            contextHandler->DestroyContext(ContextType::SelectionMenu);
            InitializeMainMenu();
        };

        Function<void> onReturnToTitle = [this, toMainMenu]() -> void {
            contextHandler->QueueOperation(toMainMenu);
        };

        userIntContext->OnReturnToTitle.Subscribe(onReturnToTitle);

        Function<void> toPaintTool = [this]() -> void {
            contextHandler->DestroyContext(ContextType::SelectionMenu);
            contextHandler->DestroyContext(ContextType::GalaxyBackground);
            fadeState = Rendering::FadeState::Out;
            InitializePaintTool();
        };

        Function<void, const std::string&> onSelectMap = [this, toPaintTool](const std::string& mapName) -> void {
            currentMapName = mapName;
            contextHandler->QueueOperation(toPaintTool);
        };

        userIntContext->OnSelectMap.Subscribe(onSelectMap);
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
        input.IsBoosting = inputManager->GetMouseInput().LeftClickHold;
        input.PhotoTogglePressed = inputManager->GetPButtonInput().ButtonExit;
        return input;
    }
} // namespace Beer::System
