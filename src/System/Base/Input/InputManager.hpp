#pragma once

#include "glm/fwd.hpp"
namespace Beer::System
{
    class InputManager
    {
    public:
        glm::vec2 GetMovementVector();
        glm::vec2 GetMouseVector();
        glm::vec2 GetMousePosition();
    };
} // namespace Beer::System
