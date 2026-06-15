#pragma once

#include <cstdint>

namespace Beer::System
{
    enum class GalaxyBrushType : uint32_t
    {
        Planet = 0,
        Asteroids = 1,
        SpaceGoo = 2,
        BlackHole = 3,
        StarDust = 4,
        Eraser = 5,
        Hand = 6,
        MagnifyingGlass = 7
    };

    constexpr static bool IsGalaxyComponent(GalaxyBrushType type)
    {
        return type != GalaxyBrushType::Eraser
            && type != GalaxyBrushType::Hand
            && type != GalaxyBrushType::MagnifyingGlass;
    }
} // namespace Beer::System
