#pragma once

#include <glm/glm.hpp>

namespace Beer::Rendering
{
    struct ModelPush
    {
        glm::mat4 ObjToWorldMat;
        glm::mat4 InvObjToWorldMat;
    };
} // namespace Beer::Rendering
