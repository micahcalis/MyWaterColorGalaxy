#pragma once

#include "glm/ext/vector_float3.hpp"
namespace Beer::System
{
    struct PlayerSettings
    {
    public:
        float MovementSpeed{};
        float Sensitivity{};
        glm::vec3 StartPos{};
    };

    static const PlayerSettings PLAYER_SETTINGS = {
        5.0f,
        0.3f,
        glm::vec3(500, 0, 500)};
} // namespace Beer::System
