#pragma once

#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Text/FontAsset.hpp"
#include "Rendering/Text/GlyphData.hpp"
#include "System/Components/UI/TextSettings.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "glm/ext/vector_float2.hpp"
#include <math.h>

namespace Beer::Rendering
{
    struct TextFormatRect
    {
    public:
        System::TextSettings Settings;
        System::PixelRect RectCorners;
        float Width;
        float Height;
        glm::vec2 Center;
        float UnitFontSize;

    public:
        TextFormatRect(System::TextSettings settings,
            System::PixelRect pixelRect,
            float fontSize)
            : Settings(settings), RectCorners(pixelRect)
        {
            Width = abs(RectCorners.TopRight.x - RectCorners.TopLeft.x);
            Height = abs(RectCorners.TopLeft.y - RectCorners.BotLeft.y);
            Center = (RectCorners.BotLeft
                         + RectCorners.BotRight
                         + RectCorners.TopLeft
                         + RectCorners.TopRight)
                * 0.25f;

            UnitFontSize = fontSize * Core::Screen::RectUnitLength();
        }
    };

    struct TextCharacter
    {
    public:
        GlyphData Glyph;
        float LocalOffset;
    };

    struct TextWord
    {
    public:
        std::vector<TextCharacter> Characters;
        float LocalLength;

        TextWord(const std::vector<uint32_t>& uniCodes,
            const FontAsset* fontAsset,
            const System::TextSettings& settings)
            : LocalLength(0)
        {
            for (auto& code : uniCodes)
            {
                const GlyphData& glyph = fontAsset->GetGlyph(code);
                Characters.emplace_back(glyph, LocalLength);
                LocalLength += glyph.Advance + settings.CharacterSpacing;
            }
        }
    };

    struct TextLine
    {
    public:
        std::vector<TextWord> Words;
        float TotalWidth;

    public:
        void AddWord(TextWord word,
            const TextFormatRect& formatRect)
        {
            TotalWidth += word.LocalLength * formatRect.UnitFontSize;
            Words.push_back(std::move(word));
        }

        void Reset()
        {
            Words.clear();
            TotalWidth = 0;
        }
    };

    class TextAlignUtilities
    {
    public:
        static std::vector<TextWord> UniCodesToWords(const std::vector<uint32_t>& uniCodes,
            const FontAsset* fontAsset,
            const System::TextSettings& settings);

        static std::vector<TextLine> DivideWordsIntoLines(const std::vector<TextWord>& words,
            const TextFormatRect& formatRect);

        static glm::vec2 GetLineStart(const TextLine& line,
            const TextFormatRect& formatRect,
            const uint32_t lineIndex,
            const uint32_t totalLines);

    private:
        static bool CanTruncateLine(const float totalHeight, const TextFormatRect& formatRect);
    };
} // namespace Beer::Rendering
