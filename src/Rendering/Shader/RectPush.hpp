#pragma once

#include <glm/glm.hpp>
#include "Rendering/Shader/Globals/PaddingMacros.hpp"

namespace Beer::System
{
    enum class AnchorMode;
}

namespace Beer::Rendering
{
    struct ALIGN_16 RectPush
    {
    public:
        glm::vec2 Pivot;
        glm::vec2 Scale;
        float Depth;
        float Rotation;
        System::AnchorMode Mode;
    };
} // namespace Beer::Rendering
