#pragma once

#include <memory>
#include "Core/Assets/MeshAsset.hpp"
#include "MeshBufferType.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include <unordered_map>

namespace Beer::Rendering
{
    struct MeshBuffers
    {
    public:
        std::shared_ptr<Buffer> IndexBuffer = nullptr;

    private:
        std::unordered_map<Rendering::MeshBufferType, std::shared_ptr<Buffer>> vertexBuffers;

    public:
        MeshBuffers(const Core::MeshAsset& meshAsset,
            const std::shared_ptr<BufferAllocator>& bufferAllocator);

        const std::shared_ptr<Buffer> GetBuffer(MeshBufferType type) const { return vertexBuffers.at(type); }

        const bool HasBuffer(MeshBufferType type) const { return vertexBuffers.at(type) != nullptr; }
        const bool HasIndex() const { return IndexBuffer != nullptr; }
    };
} // namespace Beer::Rendering
