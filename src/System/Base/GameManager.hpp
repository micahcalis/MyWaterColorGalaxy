#pragma once

#include "System/Base/Clock/ClockManager.hpp"
#include "System/Base/Input/InputManager.hpp"
#include "System/Components/Colliders/QuadColliderManager.hpp"
#include "System/Context/ContextHandler.hpp"
#include "System/Context/WorldContainer.hpp"
#include "System/Camera/CameraManager.hpp"
#include "System/Light/LightManager.hpp"
#include <memory>

namespace Beer::System
{
    class GameManager
    {
    private:
        std::unique_ptr<ClockManager> clockManager = nullptr;
        std::unique_ptr<QuadColliderManager> quadColliderManager = nullptr;
        std::unique_ptr<WorldContainer> worldContainer = nullptr;
        std::unique_ptr<ContextHandler> contextHandler = nullptr;
        std::unique_ptr<InputManager> inputManager = nullptr;
        std::unique_ptr<CameraManager> cameraManager = nullptr;
        std::unique_ptr<LightManager> lightManager = nullptr;

    public:
        void Initialize();
        void Update();

    private:
        void InitializeBase();
        void InitializeContextFactory();
        void InitializeContext();
        void InitializeColliders();
        void InitializeGalaxy();

        void UpdateBase();
        void UpdateColliders();
    };
} // namespace Beer::System
