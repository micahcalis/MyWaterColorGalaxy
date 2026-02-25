#pragma once

#include <glm/glm.hpp>
#include "vulkan/vulkan.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Beer::Rendering
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 texCoord;
        glm::vec4 color;

        static std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();

        bool operator==(const Vertex& other) const
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };

} // namespace Beer::Rendering

namespace std
{
    template<>
    struct hash<Beer::Rendering::Vertex>
    {
        size_t operator()(Beer::Rendering::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
} // namespace std
