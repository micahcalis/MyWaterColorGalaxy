#include "Rendering/Text/FontAsset.hpp"
#include "Core/Application/Managers/FontAssetManager.hpp"
#include <memory>

namespace Beer::Rendering
{
    FontAsset::FontAsset(std::shared_ptr<Texture2D> fontAtlas,
        std::unordered_map<uint32_t, GlyphData> characterMap)
        : fontAtlas(fontAtlas), characterMap(characterMap)
    {
    }

    std::shared_ptr<FontAsset> FontAsset::Get(const std::string& name)
    {
        return fontAssetManager->Get(name);
    }
} // namespace Beer::Rendering
