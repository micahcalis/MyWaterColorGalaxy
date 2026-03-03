#pragma once

#include <glm/glm.hpp>
#include "Rendering/Shader/Globals/PaddingMacros.hpp"

namespace Beer::Rendering
{
    struct ALIGN_16 EngineGlobals
    {
    public:
        const static int BINDING = 0;
        const static int DESC_COUNT = 1;

        float Time;
        float DeltaTime;
        PAD_8(0);

        ALIGN_16 glm::mat4 ViewProjMat;
        ALIGN_16 glm::mat4 InvViewProjMat;

        ALIGN_16 glm::vec3 CameraPos;
        PAD_4(1);

        ALIGN_16 glm::vec4 ScreenParams;
    };
} // namespace Beer::Rendering
