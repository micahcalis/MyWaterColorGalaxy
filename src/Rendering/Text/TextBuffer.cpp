#include "Rendering/Text/TextBuffer.hpp"
#include "Core/Assets/FontAssetLoader.hpp"
#include "FontSettings.hpp"
#include "Rendering/Text/GlyphData.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "TextAlignUtilities.hpp"
#include <cstdint>

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
        const FontSettings& fontSettings,
        const System::TextSettings& settings,
        const System::UITransform* transform)
    {
        Core::FontAssetLoader::ConvertStringToUniCode(cachedUniCodes, text);

        if (cachedUniCodes.size() > MAX_CHARACTERS)
        {
            cachedUniCodes.resize(MAX_CHARACTERS);
        }

        CalculateVertices(fontAsset,
            fontSettings,
            settings,
            transform);

        if (!cachedIndices.empty())
        {
            posBuffer->Upload(cachedPositions.data(), cachedPositions.size() * sizeof(glm::vec2));
            uvBuffer->Upload(cachedUVs.data(), cachedUVs.size() * sizeof(glm::vec2));
            indexBuffer->Upload(cachedIndices.data(), cachedIndices.size() * sizeof(uint32_t));
        }

        vertexCount = cachedPositions.size();
        indexCount = cachedIndices.size();
        cachedText = text;
    }

    MeshDrawInfo TextBuffer::GetDrawInfo() const
    {
        return MeshDrawInfo(true, vertexCount, indexCount);
    }

    void TextBuffer::CalculateVertices(const FontAsset* fontAsset,
        const FontSettings& fontSettings,
        const System::TextSettings& settings,
        const System::UITransform* transform)
    {
        cachedPositions.clear();
        cachedUVs.clear();
        cachedIndices.clear();

        cachedPositions.reserve(cachedUniCodes.size() * QUAD_VERTICES);
        cachedUVs.reserve(cachedUniCodes.size() * QUAD_VERTICES);
        cachedIndices.reserve(cachedUniCodes.size() * QUAD_INDICES);

        glm::vec2 currentPosition = glm::vec2(0);
        float atlasWidth = fontAsset->GetTexture()->GetWidth();
        float atlasHeight = fontAsset->GetTexture()->GetHeight();
        uint32_t iter = 0;

        cachedWords = TextAlignUtilities::UniCodesToWords(cachedUniCodes,
            fontAsset,
            settings);

        TextFormatRect formatRect = TextFormatRect(settings,
            transform->Rect,
            fontSettings.FontSize);

        cachedLines = TextAlignUtilities::DivideWordsIntoLines(cachedWords,
            formatRect);

        size_t lineIndex = 0;
        for (auto& line : cachedLines)
        {
            glm::vec2 currentPosition = TextAlignUtilities::GetLineStart(line,
                formatRect,
                lineIndex,
                cachedLines.size());

            for (auto& word : line.Words)
            {
                for (auto& character : word.Characters)
                {
                    const GlyphData glyph = character.Glyph;

                    float letterStartX = currentPosition.x;
                    currentPosition.x += (glyph.Advance + settings.CharacterSpacing) * formatRect.UnitFontSize;

                    if (glyph.CharacterBounds.Right == glyph.CharacterBounds.Left
                        || glyph.CharacterBounds.Top == glyph.CharacterBounds.Bottom)
                    {
                        continue;
                    }

                    float uMin = glyph.AtlasBounds.Right / atlasWidth;
                    float uMax = glyph.AtlasBounds.Left / atlasWidth;
                    float vTop = 1.0f - glyph.AtlasBounds.Top / atlasHeight;
                    float vBottom = 1.0f - glyph.AtlasBounds.Bottom / atlasHeight;

                    float xMin = letterStartX + (glyph.CharacterBounds.Left * formatRect.UnitFontSize);
                    float xMax = letterStartX + (glyph.CharacterBounds.Right * formatRect.UnitFontSize);

                    float screenYTop = currentPosition.y + (glyph.CharacterBounds.Top * formatRect.UnitFontSize);
                    float screenYBottom = currentPosition.y + (glyph.CharacterBounds.Bottom * formatRect.UnitFontSize);

                    // Top-Left
                    cachedPositions.push_back({xMin, screenYTop});
                    cachedUVs.push_back({uMax, vTop});

                    // Bottom-Left
                    cachedPositions.push_back({xMin, screenYBottom});
                    cachedUVs.push_back({uMax, vBottom});

                    // Top-Right
                    cachedPositions.push_back({xMax, screenYTop});
                    cachedUVs.push_back({uMin, vTop});

                    // Bottom-Right
                    cachedPositions.push_back({xMax, screenYBottom});
                    cachedUVs.push_back({uMin, vBottom});

                    uint32_t nIndex = iter * QUAD_VERTICES;

                    // TRI 1
                    cachedIndices.push_back(nIndex);
                    cachedIndices.push_back(nIndex + 1);
                    cachedIndices.push_back(nIndex + 2);

                    // TRI 2
                    cachedIndices.push_back(nIndex + 2);
                    cachedIndices.push_back(nIndex + 1);
                    cachedIndices.push_back(nIndex + 3);

                    iter++;
                }
            }
            lineIndex++;
        }
    }
} // namespace Beer::Rendering
