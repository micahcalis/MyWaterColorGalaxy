#pragma once

#include <glm/glm.hpp>

namespace Beer::Rendering
{
    struct TextVertex
    {
    public:
        glm::vec3 Position;
        glm::vec2 AtslasUv;
    };
} // namespace Beer::Rendering
