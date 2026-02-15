#pragma once

#include <glm/glm.hpp>
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static vk::VertexInputBindingDescription GetBindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions();
    };
} // namespace Beer::Rendering

