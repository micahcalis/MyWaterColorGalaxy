#pragma once

#include "System/Components/General/SingleMeshRender.hpp"
#include "Rendering/Mesh/MeshDrawInfo.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "System/Drawing/BindHistory.hpp"
#include "System/Drawing/BindMask.hpp"
#include "System/Drawing/BindType.hpp"

namespace Beer::System
{
    BindHistory SingleMeshRender::Bind(BindMask mask, Rendering::CommandBuffer* commandBuffer, const Rendering::ShaderPassType pass)
    {
        const Rendering::Shader* shader = material->GetShader();

        if (transform != nullptr)
        {
            Rendering::ModelPush modelPush = transform->GetShaderTransform();
            commandBuffer->BindModelPush(modelPush, shader);
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

        Rendering::MeshDrawInfo drawInfo = mesh->GetDrawInfo();
        commandBuffer->DrawMeshSingle(drawInfo);

        return BindHistory(shader, material.get(), mesh.get());
    }

    BindMask SingleMeshRender::GetBindMask(const BindHistory& history) const
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
