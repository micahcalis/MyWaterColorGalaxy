#pragma once

#include "Rendering/Buffer/Buffer.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::Rendering
{
    class QuadBuffer
    {
    private:
        std::shared_ptr<Buffer> posBuffer;
        std::shared_ptr<Buffer> uvBuffer;
        std::shared_ptr<Buffer> indexBuffer;
        uint32_t vertexCount;
        uint32_t indexCount;

        std::vector<glm::vec2> cpuPositions;
        std::vector<glm::vec2> cpuUVs;
        std::vector<uint32_t> cpuIndices;

    public:
        QuadBuffer();

        [[nodiscard]] Buffer* GetPosBuffer() const { return posBuffer.get(); }
        [[nodiscard]] Buffer* GetUVBuffer() const { return uvBuffer.get(); }
        [[nodiscard]] Buffer* GetIndexBuffer() const { return indexBuffer.get(); }

        uint32_t AddQuad(const System::PixelRect& rect);
        void Flush();
        void Clear();

    private:
        void PushVertices(const System::PixelRect& rect);
        void PushIndices();
    };
} // namespace Beer::Rendering
