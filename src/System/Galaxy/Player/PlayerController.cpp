#include "System/Galaxy/Player/PlayerController.hpp"
#include "PlayerSettings.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/quaternion.hpp"
#include <algorithm>
#include "System/Audio/LoopingStream.hpp"
#include <print>

namespace Beer::System
{
    void PlayerController::Update(PlayerInput playerInput)
    {
        SanitizeMouseInput(playerInput.MouseVec);
        Move(playerInput);
        HandleBoostAudio(playerInput.IsBoosting);
    }

    void PlayerController::Move(PlayerInput input)
    {
        Transform* transform = player->GetTransform();

        float horizontalInput = -input.MouseVec.x;
        float verticalInput = -input.MouseVec.y;

        float pitchAngle = -verticalInput * PLAYER_SETTINGS.TurnSpeed * Clock::DeltaTime();
        float rollAngle = -horizontalInput * PLAYER_SETTINGS.RollSpeed * Clock::DeltaTime();

        glm::quat pitchQuat = glm::angleAxis(pitchAngle, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat rollQuat = glm::angleAxis(rollAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        transform->Rotation = transform->Rotation * pitchQuat * rollQuat;
        transform->Rotation = glm::normalize(transform->Rotation);

        HandleSpeed(input.IsBoosting);

        transform->Position += transform->GetForward() * speed * (float)Clock::DeltaTime();
    }

    void PlayerController::HandleSpeed(bool isBoosting)
    {
        float acceleration = isBoosting ? PLAYER_SETTINGS.Acceleration : 0.0f;
        speed += acceleration * (float)Clock::DeltaTime();
        speed *= std::pow(PLAYER_SETTINGS.Drag, (float)Clock::DeltaTime());

        speed = std::clamp(speed, 0.0f, PLAYER_SETTINGS.MaxSpeed);
    }

    float PlayerController::GetNormalizedSpeed() const
    {
        return speed / PLAYER_SETTINGS.MaxSpeed;
    }

    void PlayerController::SanitizeMouseInput(glm::vec2& mouseVec)
    {
        float dt = (float)Clock::DeltaTime();
        float currentMagnitude = glm::length(mouseVec);

        if (currentMagnitude <= 0.0001f)
        {
            return;
        }

        float teleportThreshold = PLAYER_SETTINGS.TeleportThreshold * dt;
        if (currentMagnitude > teleportThreshold)
        {
            mouseVec = glm::vec2(0.0f);
            return;
        }

        float maxDeltaThisFrame = PLAYER_SETTINGS.MaxTurnRatePerSecond * dt;

        if (currentMagnitude > maxDeltaThisFrame)
        {
            mouseVec = (mouseVec / currentMagnitude) * maxDeltaThisFrame;
        }
    }

    void PlayerController::HandleBoostAudio(bool isBoosting)
    {
        if (!wasBoosting && isBoosting)
        {
            loopingStream = boostClip->GetLoop();
        }
        if (wasBoosting && !isBoosting)
        {
            if (loopingStream != nullptr)
            {
                loopingStream->Stop();
                loopingStream.reset();
            }
        }

        wasBoosting = isBoosting;
    }
} // namespace Beer::System
