#pragma once

#include "Rendering/Shader/Globals/PaddingMacros.hpp"
#include <cstdint>
#include "glm/fwd.hpp"
#include "glm/glm.hpp"

namespace Beer::System
{
    struct ALIGN_16 GalaxyDataObject
    {
    public:
        glm::vec4 ColorA = glm::vec4(0);
        glm::vec4 ColorB = glm::vec4(0);
        glm::vec4 ColorC = glm::vec4(0);
        glm::vec4 ColorD = glm::vec4(0);
        float Scale;
        uint32_t Id = 0;
        PAD_8(0);
    };
} // namespace Beer::System
