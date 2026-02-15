#include "Rendering/Vertex.hpp"
#include "vulkan/vulkan.hpp"
#include <array>
#include <cstddef>

namespace Beer::Rendering
{
    vk::VertexInputBindingDescription Vertex::GetBindingDescription()
    {
        return {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
    }

    std::array<vk::VertexInputAttributeDescription, 2> Vertex::GetAttributeDescriptions()
    {
        return {
            vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, pos)),
            vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color))};
    }

} // namespace Beer::Rendering
