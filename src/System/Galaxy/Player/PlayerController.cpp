#include "System/Galaxy/Player/PlayerController.hpp"
#include "PlayerSettings.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Beer::System
{
    void PlayerController::Update(PlayerInput playerInput)
    {
        Move(playerInput);
    }

    void PlayerController::Move(PlayerInput input)
    {
        Transform* transform = player->GetTransform();

        float horizontalInput = input.MovementVec.x;
        float verticalInput = input.MovementVec.y;

        float normalizedSpeed = GetNormalizedSpeed();
        float normalizedDt = normalizedSpeed * Clock::DeltaTime();

        float pitchAngle = -verticalInput * PLAYER_SETTINGS.TurnSpeed * normalizedDt;
        float rollAngle = -horizontalInput * PLAYER_SETTINGS.RollSpeed * normalizedDt;

        glm::quat pitchQuat = glm::angleAxis(pitchAngle, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat rollQuat = glm::angleAxis(rollAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        transform->Rotation = transform->Rotation * pitchQuat * rollQuat;
        transform->Rotation = glm::normalize(transform->Rotation);

        HandleSpeed(input.IsBoosting);

        transform->Position += transform->GetForward() * speed;
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
} // namespace Beer::System
