#pragma once

#include "Rendering/Shader/ModelPush.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "System/Delegates/Delegate.hpp"

namespace Beer::System
{
    class TextRenderComponent : public IRenderComponent
    {
    private:
        std::shared_ptr<Rendering::FontMaterial> fontMaterial;
        std::shared_ptr<Rendering::TextBuffer> textBuffer;
        Function<Rendering::RectPush> getRectPush;

    public:
        TextRenderComponent(std::shared_ptr<Rendering::FontMaterial> fontMaterial,
            Function<Rendering::RectPush> getRectPush)
            : fontMaterial(fontMaterial), getRectPush(getRectPush)
        {
            textBuffer = std::make_shared<Rendering::TextBuffer>();
        }

        void SetGetRectPush(Function<Rendering::RectPush> getRectPush) { this->getRectPush = getRectPush; }
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
    };
} // namespace Beer::System
