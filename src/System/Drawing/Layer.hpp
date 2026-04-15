#pragma once

#include "glm/fwd.hpp"

namespace Beer::System
{
    enum class Layer : uint32_t
    {
        None = 0,
        Default = 1 << 0,
        Water = 1 << 1,
        Terrain = 1 << 2,
        UI = 1 << 3,
        All = ~0u
    };

    inline constexpr uint32_t operator|(Layer a, Layer b)
    {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    inline constexpr uint32_t operator|(uint32_t mask, Layer layer)
    {
        return mask | static_cast<uint32_t>(layer);
    }

    constexpr uint32_t LAYER_NONE_BITS = static_cast<uint32_t>(Layer::None);
    constexpr uint32_t LAYER_DEFAULT_BITS = static_cast<uint32_t>(Layer::Default);
    constexpr uint32_t LAYER_WATER_BITS = static_cast<uint32_t>(Layer::Water);
    constexpr uint32_t LAYER_TERRAIN_BITS = static_cast<uint32_t>(Layer::Terrain);
    constexpr uint32_t LAYER_UI_BITS = static_cast<uint32_t>(Layer::UI);
    constexpr uint32_t LAYER_ALL_BITS = static_cast<uint32_t>(Layer::All);
} // namespace Beer::System
