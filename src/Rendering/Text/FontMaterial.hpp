#pragma once

#include "Rendering/Text/FontAsset.hpp"
#include "Rendering/Text/FontBuffer.hpp"
#include "Rendering/Text/FontSettings.hpp"
#include "Rendering/Material/IDirtyTracker.hpp"
#include <memory>

namespace Beer::Rendering
{
    class FontMaterial : public IDirtyTracker<FontMaterial>
    {
    private:
        std::shared_ptr<FontAsset> fontAsset;
        std::unique_ptr<FontBuffer> fontBuffer;
        FontSettings fontSettings{};

    public:
        FontMaterial(std::shared_ptr<FontAsset> fontAsset);
        FontMaterial(const std::string& fontName);

        Shader* GetShader() const { return FontAsset::GetTextShader(); }
        FontAsset* GetAsset() const { return fontAsset.get(); }
        FontBuffer* GetBuffer() const { return fontBuffer.get(); }
        const FontSettings& GetSettings() const { return fontSettings; }

        void Update() override;

        void SetSize(const float size);
        void SetColor(const glm::vec4 color);

        bool IsDirty() const override;

        vk::DescriptorSet GetDescriptorSet() const { return fontBuffer->GetDescriptor()->GetSet(UniformDescriptor::GetFrameIndex()); }

    private:
        void InitializeFontBuffer();
    };
} // namespace Beer::Rendering
