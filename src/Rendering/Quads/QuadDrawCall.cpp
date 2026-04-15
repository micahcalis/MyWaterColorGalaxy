#include "Rendering/Quads/QuadDrawCall.hpp"

namespace Beer::Rendering
{
    void QuadDrawCall::Execute(CommandBuffer* commandBuffer,
        const RenderContext& context,
        const ShaderPassType pass) const
    {
        if (isText)
        {
            DrawText(commandBuffer, context, pass);
        } else
        {
            DrawQuads(commandBuffer, context, pass);
        }
    }

    void QuadDrawCall::DrawQuads(CommandBuffer* commandBuffer,
        const RenderContext& context,
        const ShaderPassType pass) const
    {
        const Shader* shader = spriteMaterial->GetShader();
        const Rendering::ShaderPass* shaderPass = shader->GetPass(pass);

        commandBuffer->BindShaderPass(shader, shaderPass, context.Output);
        commandBuffer->BindMaterial(spriteMaterial);
        commandBuffer->DrawIndexedSlice(firstIndex, indexCount);
    }

    void QuadDrawCall::DrawText(CommandBuffer* commandBuffer,
        const RenderContext& context,
        const ShaderPassType pass) const
    {
        const Rendering::Shader* shader = fontMaterial->GetShader();
        const Rendering::ShaderPass* shaderPass = shader->GetPass(pass);

        commandBuffer->BindShaderPass(shader, shaderPass, context.Output);
        commandBuffer->BindFontMaterial(fontMaterial);
        commandBuffer->BindTextBuffer(textBuffer);

        Rendering::MeshDrawInfo drawInfo = textBuffer->GetDrawInfo();
        commandBuffer->DrawMeshSingle(drawInfo);
    }
} // namespace Beer::Rendering
