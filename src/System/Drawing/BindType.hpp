#pragma once

#include "glm/fwd.hpp"

namespace Beer::System
{
    enum class BindType : uint32_t
    {
        None = 0,
        Shader = 1 << 0,
        Material = 1 << 1,
        Mesh = 1 << 2,
        All = ~0u
    };

    inline constexpr uint32_t operator|(BindType a, BindType b)
    {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    inline constexpr uint32_t operator|(uint32_t mask, BindType bindType)
    {
        return mask | static_cast<uint32_t>(bindType);
    }

    constexpr uint32_t BIND_NONE_BITS = static_cast<uint32_t>(BindType::None);
    constexpr uint32_t BIND_SHADER_BITS = static_cast<uint32_t>(BindType::Shader);
    constexpr uint32_t BIND_MATERIAL_BITS = static_cast<uint32_t>(BindType::Material);
    constexpr uint32_t BIND_MESH_BITS = static_cast<uint32_t>(BindType::Mesh);
    constexpr uint32_t BIND_ALL_BITS = static_cast<uint32_t>(BindType::All);
} // namespace Beer::System
