#include "System/Components/General/MultipleMeshRender.hpp"
#include "System/Drawing/BindHistory.hpp"

namespace Beer::System
{
    BindHistory MultipleMeshRender::Bind(BindMask mask,
        Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& renderContext,
        const Rendering::ShaderPassType pass)
    {
        const Rendering::Shader* shader = material->GetShader();

        if (transforms.size() != 0)
        {
            commandBuffer->BindInstancingTransforms(transforms, renderContext, shader);
        }

        const Rendering::ShaderPass* shaderPass = shader->GetPass(pass);

        if (mask.Has(BindType::Shader))
        {
            commandBuffer->BindShaderPass(shaderPass);
        }

        if (mask.Has(BindType::Material))
        {
            commandBuffer->BindMaterial(material.get());
        }

        if (mask.Has(BindType::Mesh))
        {
            commandBuffer->BindMesh(mesh.get(), &shaderPass->BufferOrder);
        }
    }

    BindMask MultipleMeshRender::GetBindMask(const BindHistory& history) const
    {
        uint32_t bindMaskBits = 0;

        if (history.GetShader() != material->GetShader())
            bindMaskBits |= BIND_SHADER_BITS;

        if (history.GetMaterial() != material.get())
            bindMaskBits |= BIND_MATERIAL_BITS;

        if (history.GetMesh() != mesh.get())
            bindMaskBits |= BIND_MESH_BITS;

        return BindMask(bindMaskBits);
    }
} // namespace Beer::System
