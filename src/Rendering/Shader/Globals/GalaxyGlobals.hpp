#pragma once

#include <glm/glm.hpp>
#include "Rendering/Shader/Globals/PaddingMacros.hpp"
#include "glm/fwd.hpp"

namespace Beer::Rendering
{
    struct ALIGN_16 GalaxyGlobals
    {
    public:
        const static int BINDING = 2;
        const static int DESC_COUNT = 1;

        glm::vec2 MapBotLeft;
        glm::vec2 MapBotRight;
        glm::vec2 MapTopLeft;
        glm::vec2 MapTopRight;

        float MapZoomScale = 1.0f;
        PAD_4(0);
        glm::vec2 MapZoomPanning = glm::vec2(0);
    };
} // namespace Beer::Rendering
