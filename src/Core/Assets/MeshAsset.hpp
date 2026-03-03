#pragma once

#include "glm/fwd.hpp"
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include "vulkan/vulkan.hpp"
#include "Rendering/Mesh/MeshBufferType.hpp"

namespace Beer::Core
{
    struct MeshAsset
    {
    public:
        std::vector<glm::vec3> Positions;
        std::vector<glm::vec3> Normals;
        std::vector<glm::vec3> Tangents;
        std::vector<glm::vec2> UVs;
        std::vector<glm::vec4> VertexColors;
        std::vector<uint32_t> Indices;

    public:
        const static vk::DeviceSize GetImaginarySize(Rendering::MeshBufferType type, uint32_t vertexCount)
        {
            size_t elementSize = Rendering::MESH_BUFFER_SIZES[static_cast<size_t>(type)];

            switch (type)
            {
            case Rendering::MeshBufferType::Position: return elementSize * vertexCount;
            case Rendering::MeshBufferType::Normal: return elementSize * vertexCount;
            case Rendering::MeshBufferType::Tangent: return elementSize * vertexCount;
            case Rendering::MeshBufferType::Uv: return elementSize * vertexCount;
            case Rendering::MeshBufferType::Color: return elementSize * vertexCount;
            default: return 0;
            }
        }

        const vk::DeviceSize GetBufferSize(Rendering::MeshBufferType type) const
        {
            size_t elementSize = Rendering::MESH_BUFFER_SIZES[static_cast<size_t>(type)];

            switch (type)
            {
            case Rendering::MeshBufferType::Position: return elementSize * Positions.size();
            case Rendering::MeshBufferType::Normal: return elementSize * Normals.size();
            case Rendering::MeshBufferType::Tangent: return elementSize * Tangents.size();
            case Rendering::MeshBufferType::Uv: return elementSize * UVs.size();
            case Rendering::MeshBufferType::Color: return elementSize * VertexColors.size();
            default: return 0;
            }
        }

        const void* GetBufferData(Rendering::MeshBufferType type) const
        {
            switch (type)
            {
            case Rendering::MeshBufferType::Position: return Positions.data();
            case Rendering::MeshBufferType::Normal: return Normals.data();
            case Rendering::MeshBufferType::Tangent: return Tangents.data();
            case Rendering::MeshBufferType::Uv: return UVs.data();
            case Rendering::MeshBufferType::Color: return VertexColors.data();
            default: return nullptr;
            }
        }

        const vk::DeviceSize GetIndicesSize() const { return sizeof(uint32_t) * Indices.size(); }

        const vk::DeviceSize GetTotalSize() const
        {
            vk::DeviceSize totalSize = GetIndicesSize();
            uint32_t vertexCount = GetVertexCount();

            for (int i = 0; i < static_cast<int>(Rendering::MeshBufferType::Count); i++)
            {
                Rendering::MeshBufferType type = static_cast<Rendering::MeshBufferType>(i);
                vk::DeviceSize realSize = GetBufferSize(type);

                if (realSize > 0)
                {
                    totalSize += realSize;
                } else
                {
                    totalSize += GetImaginarySize(type, vertexCount);
                }
            }

            return totalSize;
        }

        uint32_t GetVertexCount() const { return Positions.size(); }
        uint32_t GetIndexCount() const { return Indices.size(); }
    };
} // namespace Beer::Core
