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
        bool LeftClickEnd;
        bool RightClickStart;
        bool RightClickHold;
        bool RightClickEnd;
        bool MiddleClickStart;
        bool MiddleClickHold;
        bool MiddleClickEnd;
        float ScrollVector;
    };
} // namespace Beer::System
