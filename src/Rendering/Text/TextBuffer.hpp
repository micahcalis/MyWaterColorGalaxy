#pragma once

#include "FontSettings.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Mesh/MeshDrawInfo.hpp"
#include "Rendering/Text/FontAsset.hpp"
#include "Rendering/Text/TextVertex.hpp"
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Beer::Rendering
{
    class TextBuffer
    {
    private:
        std::shared_ptr<Buffer> posBuffer;
        std::shared_ptr<Buffer> uvBuffer;
        std::shared_ptr<Buffer> indexBuffer;
        uint32_t vertexCount;
        uint32_t indexCount;

        std::vector<uint32_t> cachedUniCodes;
        std::vector<glm::vec2> cachedPositions;
        std::vector<glm::vec2> cachedUVs;
        std::vector<uint32_t> cachedIndices;

    public:
        TextBuffer();

        Buffer* GetPosBuffer() const { return posBuffer.get(); }
        Buffer* GetUVBuffer() const { return uvBuffer.get(); }
        Buffer* GetIndexBuffer() const { return indexBuffer.get(); }

        void Update(const std::string& text,
            const FontAsset* fontAsset,
            const FontSettings& settings);

        MeshDrawInfo GetDrawInfo() const;

    private:
        void CalculateVertices(std::vector<glm::vec2>& positions,
            std::vector<glm::vec2>& uvs,
            std::vector<uint32_t>& indices,
            const std::vector<uint32_t>& uniCodes,
            const FontAsset* fontAsset,
            const FontSettings& settings);
    };
} // namespace Beer::Rendering
