#pragma once

#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    class QuadBuffer
    {
    private:
        std::vector<std::shared_ptr<Buffer>> posBuffers;
        std::vector<std::shared_ptr<Buffer>> uvBuffers;
        std::vector<std::shared_ptr<Buffer>> indexBuffers;
        uint32_t vertexCount;
        uint32_t indexCount;

        std::vector<glm::vec2> cpuPositions;
        std::vector<glm::vec2> cpuUVs;
        std::vector<uint32_t> cpuIndices;

    public:
        QuadBuffer();

        [[nodiscard]] Buffer* GetPosBuffer() const { return posBuffers[UniformDescriptor::GetFrameIndex()].get(); }
        [[nodiscard]] Buffer* GetUVBuffer() const { return uvBuffers[UniformDescriptor::GetFrameIndex()].get(); }
        [[nodiscard]] Buffer* GetIndexBuffer() const { return indexBuffers[UniformDescriptor::GetFrameIndex()].get(); }

        uint32_t AddQuad(const System::PixelRect& rect);
        void Flush();
        void Clear();

    private:
        void PushVertices(const System::PixelRect& rect);
        void PushIndices();
    };
} // namespace Beer::Rendering
