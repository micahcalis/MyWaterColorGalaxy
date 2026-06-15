#include "System/Galaxy/Player/PlayerCamera.hpp"
#include "PlayerEntity.hpp"
#include "PlayerSettings.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Camera/CameraSettings.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_common.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include <algorithm> // For std::clamp
#include <print>

namespace Beer::System
{
    PlayerCamera::PlayerCamera(PlayerEntity* player,
        GameSubEntity* cameraEntity,
        Function<float> getPlayerNormSpeed,
        Function<bool> getPhotoMode)
        : player(player)
        , cameraEntity(cameraEntity)
        , getPlayerNormSpeed(getPlayerNormSpeed)
        , getPhotoMode(getPhotoMode)
    {
        camera = Camera::CreateCamera(CAMERA_DEFAULT_SETTINGS,
            cameraEntity->GetTransform());
    }

    void PlayerCamera::Update(PlayerInput input)
    {
        PlayerController::SanitizeMouseInput(input.MouseVec);
        RotateCamera(input.MouseVec);
        FollowPlayer();
    }

    void PlayerCamera::RotateCamera(glm::vec2 mouseVec)
    {
        float dt = static_cast<float>(Clock::DeltaTime());

        currentYawOffset -= mouseVec.x * PLAYER_CAM_SETTINGS.Sensitivity;
        currentPitchOffset -= -mouseVec.y * PLAYER_CAM_SETTINGS.Sensitivity;

        if (!getPhotoMode())
        {
            float maxAngleRadians = glm::radians(PLAYER_CAM_SETTINGS.MaxLookAngle);
            currentYawOffset = std::clamp(currentYawOffset, -maxAngleRadians, maxAngleRadians);
            currentPitchOffset = std::clamp(currentPitchOffset, -maxAngleRadians, maxAngleRadians);
        }

        if (ShouldSpringBack(mouseVec))
        {
            currentYawOffset = glm::mix(currentYawOffset, 0.0f, PLAYER_CAM_SETTINGS.SpringBackSpeed * dt);
            currentPitchOffset = glm::mix(currentPitchOffset, 0.0f, PLAYER_CAM_SETTINGS.SpringBackSpeed * dt);
        }
    }

    void PlayerCamera::FollowPlayer()
    {
        Transform* pTransform = player->GetTransform();
        Transform* cTransform = cameraEntity->GetTransform();
        float dt = static_cast<float>(Clock::DeltaTime());

        glm::quat localLookOffset = glm::quat(glm::vec3(currentPitchOffset, currentYawOffset, 0.0f));

        glm::quat targetRot = pTransform->Rotation * localLookOffset;
        glm::vec3 targetForward = targetRot * glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 targetUp = targetRot * glm::vec3(0.0f, 1.0f, 0.0f);

        float followDistance = std::lerp(PLAYER_CAM_SETTINGS.MinFollowDistance,
            PLAYER_CAM_SETTINGS.MaxFollowDistance,
            getPlayerNormSpeed());

        glm::vec3 targetPos = pTransform->Position
            - (targetForward * followDistance)
            + (targetUp * PLAYER_CAM_SETTINGS.FollowHeight);

        float posDelta = glm::clamp(PLAYER_CAM_SETTINGS.PosSmoothSpeed * dt, 0.0f, 1.0f);
        float rotDelta = glm::clamp(PLAYER_CAM_SETTINGS.RotSmoothSpeed * dt, 0.0f, 1.0f);

        camera->SetPosition(glm::mix(cTransform->Position, targetPos, posDelta));
        cTransform->Rotation = glm::slerp(cTransform->Rotation, targetRot, rotDelta);
    }

    bool PlayerCamera::ShouldSpringBack(glm::vec2 mouseVec) const
    {
        if (getPhotoMode())
        {
            return false;
        }

        float rotationOffsetLength = glm::length(glm::vec2(currentPitchOffset, currentYawOffset));

        return glm::length(mouseVec) < 0.1f;
    }
} // namespace Beer::System
