#include "Rendering/Text/FontAsset.hpp"
#include "Core/Application/Managers/FontAssetManager.hpp"

namespace Beer::Rendering
{
    std::shared_ptr<FontAsset> FontAsset::Get(const std::string& name)
    {
        return fontAssetManager->Get(name);
    }
} // namespace Beer::Rendering
