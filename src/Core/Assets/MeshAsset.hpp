#pragma once

#include "glm/fwd.hpp"
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    struct MeshAsset
    {
    public:
        std::vector<glm::vec3> Positions;
        std::vector<glm::vec2> UVs;
        std::vector<glm::vec4> VertexColors;
        std::vector<uint32_t> Indices;

    public:
        const vk::DeviceSize GetPositionsSize() const { return sizeof(glm::vec3) * Positions.size(); }
        const vk::DeviceSize GetUvsSize() const { return sizeof(glm::vec2) * UVs.size(); }
        const vk::DeviceSize GetColorsSize() const { return sizeof(glm::vec4) * VertexColors.size(); }
        const vk::DeviceSize GetIndicesSize() const { return sizeof(uint32_t) * Indices.size(); }
        const vk::DeviceSize GetTotalSize() const { return GetPositionsSize() + GetUvsSize() + GetColorsSize() + GetIndicesSize(); }
        uint32_t GetVertexCount() const { return Positions.size(); }
        uint32_t GetIndexCount() const { return Indices.size(); }
    };
} // namespace Beer::Core
