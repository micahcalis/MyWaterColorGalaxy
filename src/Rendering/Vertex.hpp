#pragma once

#include <glm/glm.hpp>
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static vk::VertexInputBindingDescription GetBindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions();
    };
} // namespace Beer::Rendering

