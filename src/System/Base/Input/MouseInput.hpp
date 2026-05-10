#pragma once

#include "glm/glm.hpp"

namespace Beer::System
{
    struct MouseInput
    {
    public:
        glm::vec2 PixelPos;
        bool leftClickStart;
        bool LeftClickHold;
        bool RightClickStart;
        bool RightClickHold;
        bool MiddleClickStart;
        bool MiddleClickHold;
        float ScrollVector;
    };
} // namespace Beer::System
