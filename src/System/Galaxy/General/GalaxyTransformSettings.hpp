#pragma once

#include "glm/ext/vector_float3.hpp"

namespace Beer::System
{
    struct GalaxyTransformSettings
    {
    public:
        uint32_t StarSeed = 0;
        uint32_t ColorSeed = 0;
        glm::vec3 SunCenter = glm::vec3(0);
        float PositionScale = 0;
        float SizeScale = 0;
        float TiltIntensity = 0;
    };
} // namespace Beer::System
