#include "Rendering/Vertex.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    std::vector<vk::VertexInputBindingDescription> Vertex::GetBindingDescriptions()
    {
        std::vector<vk::VertexInputBindingDescription> bindings(3);

        bindings[0] = {0, sizeof(glm::vec3), vk::VertexInputRate::eVertex};
        bindings[1] = {1, sizeof(glm::vec2), vk::VertexInputRate::eVertex};
        bindings[2] = {2, sizeof(glm::vec4), vk::VertexInputRate::eVertex};

        return bindings;
    }

    std::vector<vk::VertexInputAttributeDescription> Vertex::GetAttributeDescriptions()
    {
        std::vector<vk::VertexInputAttributeDescription> attributes(3);

        attributes[0] = {0, 0, vk::Format::eR32G32B32Sfloat, 0};
        attributes[1] = {1, 1, vk::Format::eR32G32Sfloat, 0};
        attributes[2] = {2, 2, vk::Format::eR32G32B32A32Sfloat, 0};

        return attributes;
    }

} // namespace Beer::Rendering

