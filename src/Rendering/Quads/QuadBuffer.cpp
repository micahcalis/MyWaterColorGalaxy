#include "Rendering/Quads/QuadBuffer.hpp"
#include "QuadBuffer.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "glm/glm.hpp"
#include <array>
#include <print>

namespace Beer::Rendering
{
    static constexpr uint32_t MAX_QUADS = 10'000;
    static constexpr uint32_t QUAD_VERTICES = 4;
    static constexpr int QUAD_INDICES = 6;

    static constexpr std::array<glm::vec2, QUAD_VERTICES> QUAD_UVS = {
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        glm::vec2(0, 0),
        glm::vec2(0, 1)};

    static constexpr size_t PIXEL_RECT_SIZE = sizeof(System::PixelRect);
    static constexpr size_t UV_RECT_SIZE = sizeof(glm::vec2) * QUAD_VERTICES;
    static constexpr size_t INDEX_RECT_SIZE = sizeof(uint32_t) * QUAD_INDICES;

    QuadBuffer::QuadBuffer()
    {
        for (int i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
        {
            posBuffers.push_back(std::make_shared<Buffer>(Buffer::CreateDynamic(
                MAX_QUADS * QUAD_VERTICES * sizeof(glm::vec2),
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)));

            uvBuffers.push_back(std::make_shared<Buffer>(Buffer::CreateDynamic(
                MAX_QUADS * QUAD_VERTICES * sizeof(glm::vec2),
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)));

            indexBuffers.push_back(std::make_shared<Buffer>(Buffer::CreateDynamic(
                MAX_QUADS * QUAD_INDICES * sizeof(uint32_t),
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)));
        }

        vertexCount = 0;
        indexCount = 0;

        cpuPositions.reserve(MAX_QUADS * QUAD_VERTICES);
        cpuUVs.reserve(MAX_QUADS * QUAD_VERTICES);
        cpuIndices.reserve(MAX_QUADS * QUAD_INDICES);
    }

    uint32_t QuadBuffer::AddQuad(const System::PixelRect& rect)
    {
        uint32_t quadFirstIndex = indexCount;

        PushVertices(rect);
        PushIndices();

        vertexCount += QUAD_VERTICES;
        indexCount += QUAD_INDICES;

        return quadFirstIndex;
    }

    void QuadBuffer::Flush()
    {
        if (vertexCount == 0)
            return;

        uint32_t frameIndex = UniformDescriptor::GetFrameIndex();

        for (int i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
        {
            posBuffers[i]->Upload(cpuPositions.data(), cpuPositions.size() * sizeof(glm::vec2));
            uvBuffers[i]->Upload(cpuUVs.data(), cpuUVs.size() * sizeof(glm::vec2));
            indexBuffers[i]->Upload(cpuIndices.data(), cpuIndices.size() * sizeof(uint32_t));
        }
    }

    void QuadBuffer::Clear()
    {
        vertexCount = 0;
        indexCount = 0;

        cpuPositions.clear();
        cpuUVs.clear();
        cpuPositions.clear();
        cpuIndices.clear();
    }

    void QuadBuffer::PushVertices(const System::PixelRect& rect)
    {
        cpuPositions.push_back(rect.TopRight);
        cpuPositions.push_back(rect.BotRight);
        cpuPositions.push_back(rect.TopLeft);
        cpuPositions.push_back(rect.BotLeft);

        for (const auto& uv : QUAD_UVS)
        {
            cpuUVs.push_back(uv);
        }
    }

    void QuadBuffer::PushIndices()
    {
        uint32_t baseVertex = vertexCount;
        cpuIndices.push_back(baseVertex + 0);
        cpuIndices.push_back(baseVertex + 1);
        cpuIndices.push_back(baseVertex + 2);
        cpuIndices.push_back(baseVertex + 2);
        cpuIndices.push_back(baseVertex + 1);
        cpuIndices.push_back(baseVertex + 3);
    }
} // namespace Beer::Rendering
