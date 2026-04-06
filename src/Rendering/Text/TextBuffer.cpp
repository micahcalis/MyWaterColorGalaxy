#include "Rendering/Text/TextBuffer.hpp"
#include "Core/Assets/FontAssetLoader.hpp"
#include "FontSettings.hpp"
#include "Rendering/Text/GlyphData.hpp"
#include "TextVertex.hpp"
#include <cstdint>

namespace Beer::Rendering
{
    static const uint32_t MAX_CHARACTERS = 10'000;
    static const uint32_t QUAD_VERTICES = 4;
    static const int QUAD_INDICES = 6;

    TextBuffer::TextBuffer()
    {
        vertexBuffer = std::make_shared<Buffer>(Buffer::CreateDynamic(
            MAX_CHARACTERS * QUAD_VERTICES * sizeof(TextVertex),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));

        indexBuffer = std::make_shared<Buffer>(Buffer::CreateDynamic(
            MAX_CHARACTERS * QUAD_INDICES * sizeof(uint32_t),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
    }

    void TextBuffer::Update(const std::string& text,
        const FontAsset* fontAsset,
        const FontSettings& settings,
        const glm::vec2 anchor,
        const float depth)
    {
        Core::FontAssetLoader::ConvertStringToUniCode(cachedUniCodes, text);

        CalculateVertices(cachedVertices,
            cachedIndices,
            cachedUniCodes,
            fontAsset,
            settings,
            anchor,
            depth);

        vertexBuffer->Upload(cachedVertices.data(), cachedVertices.size() * sizeof(TextVertex));
        indexBuffer->Upload(cachedIndices.data(), cachedIndices.size() * sizeof(uint32_t));
    }

    void TextBuffer::CalculateVertices(std::vector<TextVertex>& textVertices,
        std::vector<uint32_t>& textIndices,
        const std::vector<uint32_t>& uniCodes,
        const FontAsset* fontAsset,
        const FontSettings& settings,
        const glm::vec2 anchor,
        const float depth)
    {
        textVertices.clear();
        textVertices.reserve(uniCodes.size() * QUAD_VERTICES);

        textIndices.clear();
        textIndices.reserve(uniCodes.size() * QUAD_INDICES);

        glm::vec2 currentPosition = anchor;
        float atlasWidth = fontAsset->GetTexture()->GetWidth();
        float atlasHeight = fontAsset->GetTexture()->GetHeight();
        uint32_t iter = 0;

        for (const auto& uniCode : uniCodes)
        {
            const GlyphData& glyph = fontAsset->GetGlyph(uniCode);

            float uMin = glyph.AtlasBounds.Left / atlasWidth;
            float uMax = glyph.AtlasBounds.Right / atlasWidth;
            float vMin = glyph.AtlasBounds.Bottom / atlasHeight;
            float vMax = glyph.AtlasBounds.Top / atlasHeight;

            float xMin = currentPosition.x + (glyph.CharacterBounds.Left * settings.FontSize);
            float xMax = currentPosition.x + (glyph.CharacterBounds.Right * settings.FontSize);

            float yMin = currentPosition.y + (glyph.CharacterBounds.Bottom * settings.FontSize);
            float yMax = currentPosition.y + (glyph.CharacterBounds.Top * settings.FontSize);

            float z = depth;

            textVertices.push_back({{xMin, yMax, z}, {uMin, vMax}}); // TL
            textVertices.push_back({{xMin, yMin, z}, {uMin, vMin}}); // BL
            textVertices.push_back({{xMax, yMax, z}, {uMax, vMax}}); // TR
            textVertices.push_back({{xMax, yMin, z}, {uMax, vMin}}); // BR

            uint32_t nIndex = iter * QUAD_VERTICES;

            // TRI 1
            textIndices.push_back(nIndex);
            textIndices.push_back(nIndex + 1);
            textIndices.push_back(nIndex + 2);

            // TRI 2
            textIndices.push_back(nIndex + 2);
            textIndices.push_back(nIndex + 1);
            textIndices.push_back(nIndex + 3);

            currentPosition.x += (glyph.Advance * settings.FontSize);
            iter++;
        }
    }
} // namespace Beer::Rendering
