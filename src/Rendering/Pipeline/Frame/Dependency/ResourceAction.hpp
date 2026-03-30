#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    enum class ResourceAction : uint32_t
    {
        None = 0,
        ColorRead = 1 << 0,
        ColorWrite = 1 << 1,
        DepthWrite = 1 << 2,
        ComputeRead = 1 << 3,
        ComputeWrite = 1 << 4,
        ComputeReadWrite = 1 << 5,
        All = ~0u
    };

    inline constexpr uint32_t operator|(ResourceAction a, ResourceAction b)
    {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    inline constexpr uint32_t operator|(uint32_t mask, ResourceAction action)
    {
        return mask | static_cast<uint32_t>(action);
    }

    constexpr uint32_t RACTION_NONE_BITS = static_cast<uint32_t>(ResourceAction::None);
    constexpr uint32_t RACTION_CREAD_BITS = static_cast<uint32_t>(ResourceAction::ColorRead);
    constexpr uint32_t RACTION_CWRITE_BITS = static_cast<uint32_t>(ResourceAction::ColorWrite);
    constexpr uint32_t RACTION_ZWRITE_BITS = static_cast<uint32_t>(ResourceAction::DepthWrite);
    constexpr uint32_t RACTION_COMPREAD_BITS = static_cast<uint32_t>(ResourceAction::ComputeRead);
    constexpr uint32_t RACTION_COMPWRITE_BITS = static_cast<uint32_t>(ResourceAction::ComputeWrite);
    constexpr uint32_t RACTION_COMPREADWRITE_BITS = static_cast<uint32_t>(ResourceAction::ComputeReadWrite);
    constexpr uint32_t RACTION_ALL_BITS = static_cast<uint32_t>(ResourceAction::All);
} // namespace Beer::Rendering
