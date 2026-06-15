#pragma once

#include "BackgroundCamera.hpp"
#include "System/Camera/CameraSettings.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include <memory>

namespace Beer::System
{
    class BackgroundCameraEntity : public GameEntity
    {
    private:
        std::unique_ptr<BackgroundCamera> camera = nullptr;

    public:
        BackgroundCameraEntity(CameraSettings settings)
        {
            camera = std::make_unique<BackgroundCamera>(settings);
        }

        void Update() override
        {
            camera->Update();
        }

    private:
        void InitializeManager() override
        {
        }
    };
} // namespace Beer::System
