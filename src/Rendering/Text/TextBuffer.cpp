#include "Rendering/Text/TextBuffer.hpp"
#include "Core/Assets/FontAssetLoader.hpp"
#include "FontSettings.hpp"
#include "Rendering/Text/GlyphData.hpp"
#include <cstdint>
#include <iostream>

namespace Beer::Rendering
{
    static const uint32_t MAX_CHARACTERS = 10'000;
    static const uint32_t QUAD_VERTICES = 4;
    static const int QUAD_INDICES = 6;

    TextBuffer::TextBuffer()
    {
        posBuffer = std::make_shared<Buffer>(Buffer::CreateDynamic(
            MAX_CHARACTERS * QUAD_VERTICES * sizeof(glm::vec2),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

        uvBuffer = std::make_shared<Buffer>(Buffer::CreateDynamic(
            MAX_CHARACTERS * QUAD_VERTICES * sizeof(glm::vec2),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

        indexBuffer = std::make_shared<Buffer>(Buffer::CreateDynamic(
            MAX_CHARACTERS * QUAD_INDICES * sizeof(uint32_t),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

        vertexCount = 0;
        indexCount = 0;
    }

    void TextBuffer::Update(const std::string& text,
        const FontAsset* fontAsset,
        const FontSettings& settings)
    {
        Core::FontAssetLoader::ConvertStringToUniCode(cachedUniCodes, text);

        if (cachedUniCodes.size() > MAX_CHARACTERS)
        {
            cachedUniCodes.resize(MAX_CHARACTERS);
        }

        CalculateVertices(cachedPositions,
            cachedUVs,
            cachedIndices,
            cachedUniCodes,
            fontAsset,
            settings);

        if (!cachedIndices.empty())
        {
            posBuffer->Upload(cachedPositions.data(), cachedPositions.size() * sizeof(glm::vec2));
            uvBuffer->Upload(cachedUVs.data(), cachedUVs.size() * sizeof(glm::vec2));
            indexBuffer->Upload(cachedIndices.data(), cachedIndices.size() * sizeof(uint32_t));
        }

        vertexCount = cachedPositions.size();
        indexCount = cachedIndices.size();
    }

    MeshDrawInfo TextBuffer::GetDrawInfo() const
    {
        return MeshDrawInfo(true, vertexCount, indexCount);
    }

    void TextBuffer::CalculateVertices(std::vector<glm::vec2>& positions,
        std::vector<glm::vec2>& uvs,
        std::vector<uint32_t>& indices,
        const std::vector<uint32_t>& uniCodes,
        const FontAsset* fontAsset,
        const FontSettings& settings)
    {
        positions.clear();
        uvs.clear();
        indices.clear();

        positions.reserve(uniCodes.size() * QUAD_VERTICES);
        uvs.reserve(uniCodes.size() * QUAD_VERTICES);
        indices.reserve(uniCodes.size() * QUAD_INDICES);

        glm::vec2 currentPosition = glm::vec2(0);
        float atlasWidth = fontAsset->GetTexture()->GetWidth();
        float atlasHeight = fontAsset->GetTexture()->GetHeight();
        uint32_t iter = 0;

        for (const auto& uniCode : uniCodes)
        {
            const GlyphData& glyph = fontAsset->GetGlyph(uniCode);

            float letterStartX = currentPosition.x;
            currentPosition.x += (glyph.Advance * settings.FontSize);

            if (glyph.CharacterBounds.Right == glyph.CharacterBounds.Left
                || glyph.CharacterBounds.Top == glyph.CharacterBounds.Bottom)
            {
                continue;
            }

            float uMin = glyph.AtlasBounds.Right / atlasWidth;
            float uMax = glyph.AtlasBounds.Left / atlasWidth;
            float vTop = 1.0f - glyph.AtlasBounds.Top / atlasHeight;
            float vBottom = 1.0f - glyph.AtlasBounds.Bottom / atlasHeight;

            float xMin = letterStartX + (glyph.CharacterBounds.Left * settings.FontSize);
            float xMax = letterStartX + (glyph.CharacterBounds.Right * settings.FontSize);

            float screenYTop = currentPosition.y - (glyph.CharacterBounds.Top * settings.FontSize);
            float screenYBottom = currentPosition.y - (glyph.CharacterBounds.Bottom * settings.FontSize);

            // Top-Left
            positions.push_back({xMin, screenYTop});
            uvs.push_back({uMax, vTop});

            // Bottom-Left
            positions.push_back({xMin, screenYBottom});
            uvs.push_back({uMax, vBottom});

            // Top-Right
            positions.push_back({xMax, screenYTop});
            uvs.push_back({uMin, vTop});

            // Bottom-Right
            positions.push_back({xMax, screenYBottom});
            uvs.push_back({uMin, vBottom});

            uint32_t nIndex = iter * QUAD_VERTICES;

            // TRI 1
            indices.push_back(nIndex);
            indices.push_back(nIndex + 1);
            indices.push_back(nIndex + 2);

            // TRI 2
            indices.push_back(nIndex + 2);
            indices.push_back(nIndex + 1);
            indices.push_back(nIndex + 3);

            iter++;
        }
    }
} // namespace Beer::Rendering
