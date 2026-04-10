#include "Rendering/Text/TextAlignUtilities.hpp"
#include "System/Components/UI/TextSettings.hpp"

namespace Beer::Rendering
{
    static const int UNICODE_SPACE = 32;

    std::vector<TextWord> TextAlignUtilities::UniCodesToWords(const std::vector<uint32_t>& uniCodes,
        const FontAsset* fontAsset,
        const System::TextSettings& settings)
    {
        std::vector<TextWord> words;
        std::vector<uint32_t> currentCodes;

        for (auto& code : uniCodes)
        {
            currentCodes.push_back(code);

            if (code == UNICODE_SPACE)
            {
                words.emplace_back(currentCodes, fontAsset, settings);
                currentCodes.clear();
            }
        }

        if (currentCodes.size() != 0)
            words.emplace_back(currentCodes, fontAsset, settings);

        return words;
    }

    std::vector<TextLine> TextAlignUtilities::DivideWordsIntoLines(const std::vector<TextWord>& words,
        const TextFormatRect& formatRect)
    {
        std::vector<TextLine> lines;
        TextLine currentLine{};
        float lineHeight = formatRect.UnitFontSize * formatRect.Settings.LineSpacing;
        float totalHeight = lineHeight;

        for (auto& word : words)
        {
            float newTotalWidth = currentLine.TotalWidth + word.LocalLength * formatRect.UnitFontSize;

            if (newTotalWidth <= formatRect.Width)
            {
                currentLine.AddWord(word, formatRect);
                continue;
            }

            switch (formatRect.Settings.HorizontalWrapping)
            {
            case System::WrapMode::Truncate:
                lines.push_back(currentLine);
                totalHeight += lineHeight;

                if (!CanTruncateLine(totalHeight, formatRect))
                    return lines;

                currentLine.Reset();
                currentLine.AddWord(word, formatRect);
                break;

            case System::WrapMode::Overflow:
                continue;

            case System::WrapMode::Clip:
                lines.push_back(currentLine);
                return lines;
            }
        }

        if (currentLine.Words.size() != 0)
            lines.push_back(currentLine);

        return lines;
    };

    glm::vec2 TextAlignUtilities::GetLineStart(const TextLine& line,
        const TextFormatRect& formatRect,
        const uint32_t lineIndex,
        const uint32_t totalLines)
    {
        glm::vec2 position{};

        float lineSpacing = formatRect.UnitFontSize * formatRect.Settings.LineSpacing;
        float totalBlockHeight = totalLines * lineSpacing;

        float startY = 0.0f;

        switch (formatRect.Settings.VerticalAlignment)
        {
        case Beer::System::VerticalAlignment::Top:
            startY = formatRect.Height - formatRect.UnitFontSize;
            break;

        case Beer::System::VerticalAlignment::Middle:
            startY = (formatRect.Height * 0.5f) + (totalBlockHeight * 0.5f) - formatRect.UnitFontSize;
            break;

        case Beer::System::VerticalAlignment::Bottom:
            startY = totalBlockHeight - formatRect.UnitFontSize;
            break;
        }

        position.y = startY - (lineIndex * lineSpacing);
        float remainingWidth = formatRect.Width - line.TotalWidth;

        switch (formatRect.Settings.HorizontalAlignment)
        {
        case Beer::System::HorizontalAlignment::Left:
            position.x = 0;
            break;
        case Beer::System::HorizontalAlignment::Center:
            position.x = remainingWidth * 0.5f;
            break;
        case Beer::System::HorizontalAlignment::Right:
            position.x = remainingWidth;
            break;
        }

        return position + formatRect.RectCorners.BotLeft;
    }

    bool TextAlignUtilities::CanTruncateLine(const float totalHeight, const TextFormatRect& formatRect)
    {
        if (totalHeight <= formatRect.Height)
            return true;

        switch (formatRect.Settings.VerticalWrapping)
        {
        case System::WrapMode::Truncate:
            return false;
        case System::WrapMode::Overflow:
            return true;
        case System::WrapMode::Clip:
            return false;
        }
    }

} // namespace Beer::Rendering
