#include "System/Galaxy/Player/PlayerController.hpp"
#include "PlayerSettings.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "glm/geometric.hpp"
#include <print>

namespace Beer::System
{
    void PlayerController::Update(PlayerInput playerInput)
    {
        Rotate(playerInput.MouseVec);
        Move(playerInput.MovementVec);
    }

    void PlayerController::Move(glm::vec2 movementVec)
    {
        Transform* transform = player.Transform;

        glm::vec2 normalized = glm::normalize(movementVec);
        glm::vec3 translation = transform->GetForward() * movementVec.x;
        translation += transform->GetRight() * movementVec.y;

        transform->Position += translation
            * PLAYER_SETTINGS.MovementSpeed
            * glm::vec3(Clock::DeltaTime());

        std::println("Player Position X: {}", player.Transform->Position.x);
        std::println("Player Position Y: {}", player.Transform->Position.y);
        std::println("Player Position Z: {}", player.Transform->Position.z);
    }

    void PlayerController::Rotate(glm::vec2 mouseVec)
    {
        yaw += mouseVec.x * PLAYER_SETTINGS.Sensitivity;
        pitch -= mouseVec.y * PLAYER_SETTINGS.Sensitivity;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        front = glm::normalize(front);

        glm::vec3 right = glm::normalize(glm::cross(front, WORLD_UP));
        glm::vec3 up = glm::normalize(glm::cross(right, front));

        // glm::mat4 viewMatrix = glm::lookAt(
        //     position,         // Where the camera is
        //     position + front, // What the camera is looking at
        //     up                // Which way is up for the camera
        // );
    }
} // namespace Beer::System
