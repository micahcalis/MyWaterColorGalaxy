#pragma once

#include "glm/glm.hpp"

namespace Beer::Rendering
{
    struct FontSettings
    {
    public:
        float FontSize = 1;
        glm::vec4 FontColor{};
    };
} // namespace Beer::Rendering
