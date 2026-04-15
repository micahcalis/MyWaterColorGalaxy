#pragma once

#include <array>
#include <glm/glm.hpp>

namespace Beer::Rendering
{
    enum class MeshBufferType
    {
        Position,
        Normal,
        Tangent,
        Uv,
        Color,
        Count,
        Position2D,
    };

    constexpr std::array<size_t, static_cast<size_t>(MeshBufferType::Count)> MESH_BUFFER_SIZES = {
        sizeof(glm::vec3), // Position
        sizeof(glm::vec3), // Normal
        sizeof(glm::vec3), // Tangent
        sizeof(glm::vec2), // Uv
        sizeof(glm::vec4)  // Color
    };
} // namespace Beer::Rendering
