#pragma once

#include "Rendering/Shader/Globals/PaddingMacros.hpp"
#include "glm/glm.hpp"

namespace Beer::Rendering
{
    struct ALIGN_16 FontSettings
    {
    public:
        float FontSize = 1;
        PAD_12(0);
        glm::vec4 FontColor{};
    };
} // namespace Beer::Rendering
