#pragma once

#include "System/Base/Clock/Clock.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Camera/CameraSettings.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include <memory>
namespace Beer::System
{
    static const float BG_CAM_PITCH_SPEED = -0.002f;
    static const float BG_CAM_YAW_SPEED = -0.002f;

    class BackgroundCamera
    {
    private:
        std::unique_ptr<Camera> camera = nullptr;
        std::unique_ptr<GameSubEntity> cameraContainer = nullptr;

        float pitch = 0;
        float yaw = 0;

    public:
        BackgroundCamera(CameraSettings settings)
        {
            Transform cameraTransform{};
            cameraContainer = std::make_unique<GameSubEntity>(cameraTransform);
            camera = Camera::CreateCamera(settings, cameraContainer->GetTransform());
        }

        void Update()
        {
            pitch = BG_CAM_PITCH_SPEED * Clock::DeltaTime();
            yaw = BG_CAM_YAW_SPEED * Clock::DeltaTime();

            glm::quat lookOffset = glm::quat(glm::vec3(pitch, yaw, 0));
            Transform* cameraTransform = cameraContainer->GetTransform();
            cameraTransform->Rotation = cameraTransform->Rotation * lookOffset;
        }
    };
} // namespace Beer::System
