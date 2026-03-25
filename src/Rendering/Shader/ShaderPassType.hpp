#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    enum class ShaderPassType : uint32_t
    {
        Unknown = 0,
        Opaque = 1 << 0,
        Transparent = 1 << 1,
        Skybox = 1 << 2
    };

    inline constexpr uint32_t operator|(ShaderPassType a, ShaderPassType b)
    {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    inline constexpr uint32_t operator|(uint32_t mask, ShaderPassType layer)
    {
        return mask | static_cast<uint32_t>(layer);
    }

    constexpr uint32_t PASS_UNKNOWN_BITS = static_cast<uint32_t>(ShaderPassType::Unknown);
    constexpr uint32_t PASS_OPAQUE_BITS = static_cast<uint32_t>(ShaderPassType::Opaque);
    constexpr uint32_t PASS_TRANSPARENT_BITS = static_cast<uint32_t>(ShaderPassType::Transparent);
    constexpr uint32_t PASS_SKYBOX_BITS = static_cast<uint32_t>(ShaderPassType::Skybox);
} // namespace Beer::Rendering
