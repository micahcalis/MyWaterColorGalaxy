#pragma once

#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "System/Base/Clock/ClockManager.hpp"
#include "System/Base/Input/InputManager.hpp"
#include "System/Components/Colliders/QuadColliderManager.hpp"
#include "System/Context/ContextHandler.hpp"
#include "System/Context/WorldContainer.hpp"
#include "System/Camera/CameraManager.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include "System/Light/LightManager.hpp"
#include "System/Serialization/MapSerializationManager.hpp"
#include <memory>

namespace Beer::System
{
    class GameManager
    {
    private:
        Function<void> quitApplication = nullptr;
        std::unique_ptr<ClockManager> clockManager = nullptr;
        std::unique_ptr<QuadColliderManager> quadColliderManager = nullptr;
        std::unique_ptr<WorldContainer> worldContainer = nullptr;
        std::unique_ptr<ContextHandler> contextHandler = nullptr;
        std::unique_ptr<InputManager> inputManager = nullptr;
        std::unique_ptr<CameraManager> cameraManager = nullptr;
        std::unique_ptr<LightManager> lightManager = nullptr;
        std::unique_ptr<MapSerializationManager> mapSerializationManager = nullptr;

        std::string currentMapName = "FallbackMap";
        Rendering::FadeState fadeState = Rendering::FadeState::Out;

    public:
        ~GameManager();
        GameManager(Function<void> quitApplication);
        void Initialize();
        void PreUpdate();
        void Update();
        void OnMouseScrolled(float scrollY);

    private:
        void InitializeBase();
        void InitializeContextFactory();
        void InitializeContext();
        void InitializeColliders();
        void InitializeSerialization();
        void InitializeGalaxy();
        void InitializePaintTool();
        void InitializeMainMenu();
        void InitializeSelectMenu();

        void UpdateBase();
        void UpdateColliders();
        PlayerInput GetPlayerInput();
    };
} // namespace Beer::System
