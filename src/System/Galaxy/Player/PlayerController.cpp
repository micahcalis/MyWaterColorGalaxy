#include "System/Galaxy/Player/PlayerController.hpp"
#include "PlayerSettings.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Camera/Camera.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

namespace Beer::System
{
    void PlayerController::Update(PlayerInput playerInput)
    {
        Move(playerInput.MovementVec);
    }

    void PlayerController::Move(glm::vec2 movementVec)
    {
        Transform* transform = player->GetTransform();

        if (Camera::Main() == nullptr)
            return;

        const Transform* cameraTransform = Camera::Main()->GetTransform();

        glm::vec2 normalized = glm::normalize(movementVec);
        glm::vec3 translation = cameraTransform->GetForward() * movementVec.x;
        translation += cameraTransform->GetRight() * movementVec.y;

        transform->Position = transform->Position
            + translation
                * PLAYER_SETTINGS.MovementSpeed
                * glm::vec3(Clock::DeltaTime());
    }
} // namespace Beer::System
