#include "System/Components/General/MultipleMeshRender.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Shader/Globals/ModelTransformData.hpp"
#include "System/Drawing/BindHistory.hpp"

namespace Beer::System
{
    BindHistory MultipleMeshRender::Bind(BindMask mask,
        Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& renderContext,
        const Rendering::ShaderPassType pass)
    {
        const Rendering::Shader* shader = material->GetShader();
        std::vector<Rendering::ModelTransformData> modelTransformDatas;

        if (getModelTransformData != nullptr)
        {
            modelTransformDatas = getModelTransformData();
            commandBuffer->BindInstancingTransforms(modelTransformDatas, renderContext, shader);
        }

        const Rendering::ShaderPass* shaderPass = shader->GetPass(pass);

        if (mask.Has(BindType::Shader))
        {
            commandBuffer->BindShaderPass(material->GetShader(), shaderPass, renderContext.Output);
        }

        if (mask.Has(BindType::Material))
        {
            commandBuffer->BindMaterial(material.get());
        }

        if (mask.Has(BindType::Mesh))
        {
            commandBuffer->BindMesh(mesh.get(), &shaderPass->Input.BufferOrder);
        }

        Rendering::MeshDrawInfo drawInfo = mesh->GetDrawInfo();
        commandBuffer->DrawMeshMultiple(drawInfo, modelTransformDatas.size());

        return BindHistory(shader, material.get(), mesh.get());
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
