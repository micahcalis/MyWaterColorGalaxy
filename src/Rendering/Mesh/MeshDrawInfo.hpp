#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    struct MeshDrawInfo
    {
    private:
        bool canIndex;
        uint32_t vertexCount;
        uint32_t indexCount;

    public:
        MeshDrawInfo(bool canIndex,
            uint32_t vertexCount,
            uint32_t indexCount)
            : canIndex(canIndex), vertexCount(vertexCount), indexCount(indexCount)
        {
        }

        bool CanIndex() const { return canIndex; }
        uint32_t GetVertexCount() const { return vertexCount; }
        uint32_t GetIndexCount() const { return indexCount; }
    };
} // namespace Beer::Rendering
