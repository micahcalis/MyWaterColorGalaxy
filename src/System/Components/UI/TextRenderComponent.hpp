#pragma once

#include "Rendering/Text/TextBuffer.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "System/Delegates/Delegate.hpp"
#include "TextSettings.hpp"
#include "UITransform.hpp"

namespace Beer::System
{
    class TextRenderComponent : public IRenderComponent
    {
    private:
        std::shared_ptr<Rendering::FontMaterial> fontMaterial;
        std::shared_ptr<Rendering::TextBuffer> textBuffer;
        Function<UITransform*> getTransform;
        TextSettings textSettings;

        std::string cachedText = "";

    public:
        TextRenderComponent(std::shared_ptr<Rendering::FontMaterial> fontMaterial,
            Function<UITransform*> getTransform,
            TextSettings textSettings = TextSettings())
            : fontMaterial(fontMaterial), getTransform(getTransform), textSettings(textSettings)
        {
            textBuffer = std::make_shared<Rendering::TextBuffer>();
        }

        void SetGetTransform(Function<UITransform*> getTransform) { this->getTransform = getTransform; }
        void SetText(const std::string& text);

        BindHistory Bind(BindMask mask,
            Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            const Rendering::ShaderPassType pass) override;

        bool HasPass(Rendering::ShaderPassType pass) const override { return fontMaterial->GetShader()->HasPass(pass); }
        Layer GetLayer() const override { return Layer::UI; }
        BindMask GetBindMask(const BindHistory& history) const override;

        const Rendering::Shader* GetPrimaryShader() const override { return fontMaterial->GetShader(); }
        const Rendering::Material* GetPrimaryMaterial() const override { return nullptr; }
        const Rendering::Mesh* GetPrimaryMesh() const override { return nullptr; }

        void SetHorizontalAlignment(const HorizontalAlignment alignment) { textSettings.HorizontalAlignment = alignment; }
        void SetVerticalAlignment(const VerticalAlignment alignment) { textSettings.VerticalAlignment = alignment; }
        void SetHorizontalWrapping(const WrapMode wrapMode) { textSettings.HorizontalWrapping = wrapMode; }
        void SetVerticalWrapping(const WrapMode wrapMode) { textSettings.VerticalWrapping = wrapMode; }
        void SetCharacterSpacing(const float spacing) { textSettings.CharacterSpacing = spacing; }
        void SetLineSpacing(const float spacing) { textSettings.LineSpacing = spacing; }
    };
} // namespace Beer::System
