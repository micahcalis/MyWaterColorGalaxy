#pragma once

#include "FontSettings.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Text/FontAsset.hpp"
#include "TextVertex.hpp"

namespace Beer::Rendering
{
    class TextBuffer
    {
    private:
        std::shared_ptr<Buffer> vertexBuffer;
        std::shared_ptr<Buffer> indexBuffer;

        std::vector<uint32_t> cachedUniCodes;
        std::vector<TextVertex> cachedVertices;
        std::vector<uint32_t> cachedIndices;

    public:
        TextBuffer();

        void Update(const std::string& text,
            const FontAsset* fontAsset,
            const FontSettings& settings,
            const glm::vec2 anchor,
            const float depth);

    private:
        void CalculateVertices(std::vector<TextVertex>& textVertices,
            std::vector<uint32_t>& textIndices,
            const std::vector<uint32_t>& uniCodes,
            const FontAsset* fontAsset,
            const FontSettings& settings,
            const glm::vec2 anchor,
            const float depth);
    };
} // namespace Beer::Rendering
