#pragma once

#include "Rendering/Shader/Shader.hpp"
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
    static const uint32_t FALLBACK_KEYCODE = 42;

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
        static Shader* GetTextShader();

        FontAsset(std::shared_ptr<Texture2D> fontAtlas,
            std::unordered_map<uint32_t, GlyphData> characterMap);

        const GlyphData& GetGlyph(uint32_t uniCode) const
        {
            auto it = characterMap.find(uniCode);

            if (it != characterMap.end())
            {
                return it->second;
            }

            auto fallbackIt = characterMap.find(FALLBACK_KEYCODE);
            if (fallbackIt != characterMap.end())
            {
                return fallbackIt->second;
            }

            static const GlyphData emptyDummyGlyph{};
            return emptyDummyGlyph;
        }

        Texture2D* GetTexture() const { return fontAtlas.get(); }
    };
} // namespace Beer::Rendering
