#include "Rendering/Text/FontMaterial.hpp"
#include <memory>

namespace Beer::Rendering
{
    FontMaterial::FontMaterial(std::shared_ptr<FontAsset> fontAsset)
        : fontAsset(fontAsset)
    {
        InitializeFontBuffer();
    }

    FontMaterial::FontMaterial(const std::string& fontName)
    {
        fontAsset = FontAsset::Get(fontName);
        InitializeFontBuffer();
    }

    void FontMaterial::Update()
    {
        if (dirtyFramesCountBuffer > 0)
        {
            fontBuffer->Update(fontSettings);
            dirtyFramesCountBuffer--;
        }
    }
    void FontMaterial::SetSize(const float size)
    {
        fontSettings.FontSize = size;
        MarkDirty();
    }

    void FontMaterial::SetColor(const glm::vec4 color)
    {
        fontSettings.FontColor = color;
        MarkDirty();
    }

    void FontMaterial::InitializeFontBuffer()
    {
        fontBuffer = std::make_unique<FontBuffer>(fontAsset->GetTexture());
    }

    bool FontMaterial::IsDirty() const
    {
        return dirtyFramesCountBuffer > 0;
    }
} // namespace Beer::Rendering
