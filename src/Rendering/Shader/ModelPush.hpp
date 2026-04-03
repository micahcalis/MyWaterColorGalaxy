#pragma once

#include <glm/glm.hpp>
#include "Rendering/Shader/Globals/PaddingMacros.hpp"

namespace Beer::Rendering
{
    const int32_t IS_INSTANCED = 1;
    const int32_t IS_NOT_INSTANCED = 0;

    struct ALIGN_16 ModelPush
    {
    public:
        glm::mat3x4 ObjToWorldMat;
        glm::mat3x4 ObjToWorldDirMat;
        int32_t IsInstanced;

    public:
        ModelPush(const glm::mat4& objToWorldMat, const glm::mat4& objToWorldDirMat, bool isInstanced)
            : ObjToWorldMat(PackMatrix(objToWorldMat)), ObjToWorldDirMat(PackMatrix(objToWorldDirMat)), IsInstanced(isInstanced ? IS_INSTANCED : IS_NOT_INSTANCED)
        {
        }

    private:
        glm::mat3x4 PackMatrix(const glm::mat4& m)
        {
            glm::mat4 transposed = glm::transpose(m);
            return glm::mat3x4(transposed[0], transposed[1], transposed[2]);
        }
    };
} // namespace Beer::Rendering
