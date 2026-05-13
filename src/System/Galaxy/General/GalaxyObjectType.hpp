#pragma once

#include <cstdint>

namespace Beer::System
{
    static const uint32_t NUM_GALAXY_OBJ_TYPES = 5;

    enum class GalaxyObjectType : uint32_t
    {
        Planet = 0,
        Asteroids = 1,
        SpaceGoo = 2,
        BlackHole = 3,
        StarDust = 4
    };
} // namespace Beer::System
