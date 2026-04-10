#include "System/Components/UI/TextRenderComponent.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
#include "System/Drawing/BindHistory.hpp"

namespace Beer::System
{
    void TextRenderComponent::SetText(const std::string& text)
    {
        if (cachedText == text)
            return;

        textBuffer->Update(text,
            fontMaterial->GetAsset(),
            fontMaterial->GetSettings(),
            textSettings,
            getTransform());

        cachedText = text;
    }

    void TextRenderComponent::ReloadTextBuffer()
    {
        textBuffer->Reload(fontMaterial->GetAsset(),
            fontMaterial->GetSettings(),
            textSettings,
            getTransform());
    }

    BindHistory TextRenderComponent::Bind(BindMask mask,
        Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& renderContext,
        const Rendering::ShaderPassType pass)
    {
        const Rendering::Shader* shader = fontMaterial->GetShader();

        if (getTransform != nullptr)
        {
            commandBuffer->BindRectPush(getTransform()->Rect.GetRectPush(), shader);
        }

        const Rendering::ShaderPass* shaderPass = shader->GetPass(pass);

        if (mask.Has(BindType::Shader))
        {
            commandBuffer->BindShaderPass(shader, shaderPass, renderContext.Output);
        }

        commandBuffer->BindFontMaterial(fontMaterial.get());
        commandBuffer->BindTextBuffer(textBuffer.get());

        Rendering::MeshDrawInfo drawInfo = textBuffer->GetDrawInfo();
        commandBuffer->DrawMeshSingle(drawInfo);

        return BindHistory(shader, nullptr, nullptr);
    }

    BindMask TextRenderComponent::GetBindMask(const BindHistory& history) const
    {
        uint32_t bindMaskBits = 0;

        if (history.GetShader() != fontMaterial->GetShader())
            bindMaskBits |= BIND_SHADER_BITS;

        bindMaskBits |= BIND_MATERIAL_BITS;
        bindMaskBits |= BIND_MESH_BITS;

        return BindMask(bindMaskBits);
    }
} // namespace Beer::System
