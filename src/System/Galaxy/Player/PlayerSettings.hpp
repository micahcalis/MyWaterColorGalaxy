#pragma once

namespace Beer::System
{
    struct PlayerSettings
    {
    public:
        float MovementSpeed;
        float Sensitivity;
    };

    static const PlayerSettings PLAYER_SETTINGS = {
        5.0f,
        0.1f};
} // namespace Beer::System
