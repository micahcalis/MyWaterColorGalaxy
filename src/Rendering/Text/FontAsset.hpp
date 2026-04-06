#pragma once

#include "Rendering/Text/GlyphData.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace Beer::Core
{
    class FontAssetManager;
}

namespace Beer::Rendering
{
    class FontAsset
    {
    private:
        inline static Core::FontAssetManager* fontAssetManager = nullptr;

        std::shared_ptr<Texture2D> fontAtlas;
        std::unordered_map<uint32_t, GlyphData> characterMap;

    public:
        static void SetFontAssetManager(Core::FontAssetManager* fontAssetManager)
        {
            FontAsset::fontAssetManager = fontAssetManager;
        }

        static std::shared_ptr<FontAsset> Get(const std::string& name);

        FontAsset(std::shared_ptr<Texture2D> fontAtlas,
            std::unordered_map<uint32_t, GlyphData> characterMap)
            : fontAtlas(fontAtlas), characterMap(characterMap)
        {
        }
    };
} // namespace Beer::Rendering
