#pragma once

#include <vector>
#include "vulkan/vulkan.hpp"
#include "Rendering/Mesh/MeshBufferOrder.hpp"
#

namespace Beer::Rendering
{
    struct VertexInput
    {
        std::vector<vk::VertexInputBindingDescription> BindingDescs;
        std::vector<vk::VertexInputAttributeDescription> AttributeDescs;
        MeshBufferOrder BufferOrder;
    };
} // namespace Beer::Rendering
