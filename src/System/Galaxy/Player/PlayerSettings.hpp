#pragma once

#include "glm/ext/vector_float3.hpp"
namespace Beer::System
{
    struct PlayerSettings
    {
    public:
        float Acceleration{};
        float MaxSpeed{};
        float Drag{};
        float Sensitivity{};
        float TurnSpeed{};
        float RollSpeed{};
        float YawSpeed{};
        float FadeSpeed{};
        float TeleportThreshold{};
        float MaxTurnRatePerSecond{};
    };

    static const PlayerSettings PLAYER_SETTINGS = {
        25.0f,
        40.0f,
        0.65f,
        0.3f,
        1.75f,
        1.75f,
        1.75f,
        2.75f,
        12500.0f,
        2500.0f};

    struct PlayerCameraSettings
    {
    public:
        float Sensitivity{};
        float MinFollowDistance{};
        float MaxFollowDistance{};
        float FollowHeight{};
        float PosSmoothSpeed{};
        float RotSmoothSpeed{};
        float SpringBackSpeed{};
        float MaxLookAngle{};
        float SpringDeadzone{};
    };

    static const PlayerCameraSettings PLAYER_CAM_SETTINGS{
        0.005f,
        15.0f,
        26.0f,
        2.5f,
        5.0f,
        5.0f,
        1.5f,
        90.0f,
        45.0f};
} // namespace Beer::System
