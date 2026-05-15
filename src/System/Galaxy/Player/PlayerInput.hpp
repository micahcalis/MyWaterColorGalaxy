#pragma once

#include "glm/glm.hpp"

namespace Beer::System
{
    struct PlayerInput
    {
        glm::vec2 MovementVec{};
        glm::vec2 MouseVec{};
        bool IsBoosting{};
    };
} // namespace Beer::System
