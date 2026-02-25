#pragma once

#include <memory>
#include "Rendering/Buffer/Buffer.hpp"

namespace Beer::Rendering
{
    struct MeshBuffers
    {
    public:
        std::shared_ptr<Buffer> PositionBuffer = nullptr;
        std::shared_ptr<Buffer> UvBuffer = nullptr;
        std::shared_ptr<Buffer> ColorBuffer = nullptr;
        std::shared_ptr<Buffer> IndexBuffer = nullptr;

    public:
        const bool HasPositions() const { return PositionBuffer != nullptr; }
        const bool HasUv() const { return UvBuffer != nullptr; }
        const bool HasColor() const { return ColorBuffer != nullptr; }
        const bool HasIndex() const { return IndexBuffer != nullptr; }
    };
} // namespace Beer::Rendering
