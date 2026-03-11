#include "System/Galaxy/Player/PlayerCamera.hpp"
#include "PlayerEntity.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Camera/CameraSettings.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include "System/Context/ContextType.hpp"
#include "glm/ext/vector_float3.hpp"

namespace Beer::System
{
    const float PLAYER_CAM_FOLLOW_SPEED = 5.0f;

    PlayerCamera::PlayerCamera(PlayerEntity* player, GameSubEntity* cameraEntity)
        : player(player), cameraEntity(cameraEntity)
    {
        camera = Camera::CreateCamera(CAMERA_DEFAULT_SETTINGS,
            cameraEntity->GetTransform());
    }

    void PlayerCamera::Update(PlayerInput input)
    {
        FollowPlayer();
        RotateCamera(input.MouseVec);
    }

    void PlayerCamera::FollowPlayer()
    {
        float delta = PLAYER_CAM_FOLLOW_SPEED * Clock::DeltaTime();
        const Transform* transform = camera->GetTransform();

        camera->SetPosition(glm::mix(transform->Position,
            player->GetTransform()->Position,
            delta));
    }

    void PlayerCamera::RotateCamera(glm::vec2 mouseVec)
    {
        yaw += mouseVec.x * PLAYER_SETTINGS.Sensitivity;
        pitch += mouseVec.y * PLAYER_SETTINGS.Sensitivity;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 forward;
        forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        forward.y = sin(glm::radians(pitch));
        forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        forward = glm::normalize(forward);

        glm::vec3 right = glm::normalize(glm::cross(forward, WORLD_UP));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));

        camera->SetOrientation(up, forward);
    }
} // namespace Beer::System
