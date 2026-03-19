#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    enum class ResourceAction : uint32_t
    {
        None = 0,
        Read = 1 << 0,
        Write = 1 << 1,
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
    constexpr uint32_t RACTION_READ_BITS = static_cast<uint32_t>(ResourceAction::Read);
    constexpr uint32_t RACTION_WRITE_BITS = static_cast<uint32_t>(ResourceAction::Write);
    constexpr uint32_t RACTION_ALL_BITS = static_cast<uint32_t>(ResourceAction::All);
} // namespace Beer::Rendering
