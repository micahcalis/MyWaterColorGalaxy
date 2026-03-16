#pragma once

#include <glm/glm.hpp>
#include "Rendering/Shader/Globals/PaddingMacros.hpp"

namespace Beer::Rendering
{
    struct ALIGN_16 LightingGlobals
    {
    public:
        const static int BINDING = 1;
        const static int DESC_COUNT = 1;

        glm::vec4 MainLightColor;
        glm::vec4 ShadowColor;
        glm::vec4 SkyColor;
        glm::vec3 MainLightPos;
        PAD_4(0);
    };
} // namespace Beer::Rendering
