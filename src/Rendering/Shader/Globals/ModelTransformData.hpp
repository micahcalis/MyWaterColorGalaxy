#pragma once

#include <glm/glm.hpp>
#include "Rendering/Shader/Globals/PaddingMacros.hpp"

namespace Beer::Rendering
{
    struct ALIGN_16 ModelTransformData
    {
    public:
        const static int BINDING = 0;
        const static int DESC_COUNT = 1;

        glm::mat4x4 ObjToWorldMat;
        glm::mat4x4 ObjToWorldDirMat;
    };
} // namespace Beer::Rendering
