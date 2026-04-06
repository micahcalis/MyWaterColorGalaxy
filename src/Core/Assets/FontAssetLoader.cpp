#include "Core/Assets/FontAssetLoader.hpp"
#include "Rendering/Text/GlyphData.hpp"

#include <fstream>
#include <unordered_map>
#include <vector>
#include "Vendor/nlohmann/json.hpp"

namespace Beer::Core
{
    void FontAssetLoader::ConvertStringToUniCode(std::vector<uint32_t>& codepoints,
        const std::string& text)
    {
        codepoints.clear();
        codepoints.reserve(text.size());

        size_t i = 0;
        size_t len = text.length();

        while (i < len)
        {
            unsigned char c0 = static_cast<unsigned char>(text[i]);
            uint32_t codepoint = 0;

            if ((c0 & 0x80) == 0)
            {
                codepoint = c0;
                i += 1;
            } else if ((c0 & 0xE0) == 0xC0 && i + 1 < len)
            {
                codepoint = (c0 & 0x1F) << 6;
                codepoint |= (static_cast<unsigned char>(text[i + 1]) & 0x3F);
                i += 2;
            } else if ((c0 & 0xF0) == 0xE0 && i + 2 < len)
            {
                codepoint = (c0 & 0x0F) << 12;
                codepoint |= (static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6;
                codepoint |= (static_cast<unsigned char>(text[i + 2]) & 0x3F);
                i += 3;
            } else if ((c0 & 0xF8) == 0xF0 && i + 3 < len)
            {
                codepoint = (c0 & 0x07) << 18;
                codepoint |= (static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12;
                codepoint |= (static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6;
                codepoint |= (static_cast<unsigned char>(text[i + 3]) & 0x3F);
                i += 4;
            } else
            {
                codepoint = 0xFFFD;
                i += 1;
            }

            codepoints.push_back(codepoint);
        }
    }

    std::unordered_map<uint32_t, Rendering::GlyphData> FontAssetLoader::ReadCharacterMap(const std::filesystem::path& jsonPath)
    {
        std::unordered_map<uint32_t, Rendering::GlyphData> characterMap;

        std::ifstream file(jsonPath);

        if (!file.is_open())
            throw std::runtime_error("Failed to open Font JSON: " + jsonPath.string());

        nlohmann::json j;
        file >> j;

        for (const auto& jsonGlyph : j["glyphs"])
        {
            uint32_t unicode = jsonGlyph["unicode"];

            Rendering::GlyphData data{};
            data.Advance = jsonGlyph["advance"];

            if (jsonGlyph.contains("planeBounds"))
            {
                data.CharacterBounds = {
                    jsonGlyph["planeBounds"]["left"],
                    jsonGlyph["planeBounds"]["bottom"],
                    jsonGlyph["planeBounds"]["right"],
                    jsonGlyph["planeBounds"]["top"]};

                data.AtlasBounds = {
                    jsonGlyph["atlasBounds"]["left"],
                    jsonGlyph["atlasBounds"]["bottom"],
                    jsonGlyph["atlasBounds"]["right"],
                    jsonGlyph["atlasBounds"]["top"]};
            } else
            {
                data.CharacterBounds = {0.0f, 0.0f, 0.0f, 0.0f};
                data.AtlasBounds = {0.0f, 0.0f, 0.0f, 0.0f};
            }

            characterMap[unicode] = data;
        }

        return characterMap;
    }
} // namespace Beer::Core
