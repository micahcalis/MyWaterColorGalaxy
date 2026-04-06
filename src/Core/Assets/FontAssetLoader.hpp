#pragma once

#include "Rendering/Text/GlyphData.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>

namespace Beer::Core
{
    class FontAssetLoader
    {
    public:
        static std::vector<uint32_t> ConvertStringToUniCode(const std::string& text);
        static std::unordered_map<uint32_t, Rendering::GlyphData> ReadCharacterMap(const std::filesystem::path& jsonPath);
    };
} // namespace Beer::Core
