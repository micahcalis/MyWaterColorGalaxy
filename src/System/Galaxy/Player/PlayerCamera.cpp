#include "System/Galaxy/Player/PlayerCamera.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Camera/CameraSettings.hpp"
#include "System/Context/ContextType.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/geometric.hpp"
#include <cmath>
#include <print>

namespace Beer::System
{
    const float PLAYER_CAM_FOLLOW_SPEED = 5.0f;

    PlayerCamera::PlayerCamera(PlayerHandle player)
        : player(player)
    {
        camera = Camera::CreateCamera(CAMERA_DEFAULT_SETTINGS,
            ContextType::Galaxy);
    }

    void PlayerCamera::Update()
    {
        FollowPlayer();
    }

    void PlayerCamera::FollowPlayer()
    {
        float delta = PLAYER_CAM_FOLLOW_SPEED * Clock::DeltaTime();
        const Transform* transform = camera->GetTransform();
        glm::vec3 oldPos = transform->Position;

        camera->SetPosition(glm::mix(
            transform->Position,
            player.Transform->Position,
            delta));

        camera->SetOrientation(player.Transform->Rotation);

        std::println("Velocity Length: {}", glm::distance(oldPos, transform->Position));
    }
} // namespace Beer::System
