#pragma once

#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Drawing/BindHistory.hpp"
#include "System/Drawing/BindMask.hpp"
#include "System/Drawing/BindType.hpp"

namespace Beer::System
{
    BindHistory SingleMeshRender::Bind(BindMask mask, vk::CommandBuffer commandBuffer, const Rendering::ShaderPassType pass)
    {
        const Rendering::Shader* shader = material->GetShader();

        if (mask.Has(BindType::Shader))
        {
            shader->BindPass(commandBuffer, pass);
        }

        if (mask.Has(BindType::Material))
        {
            material->BindBuffer(commandBuffer);
        }

        bool canIndex;

        if (mask.Has(BindType::Mesh))
        {
            mesh->Bind(commandBuffer, shader->GetPass(pass)->BufferOrder, canIndex);
        }

        canIndex = mesh->GetBuffers().HasIndex();

        if (canIndex)
        {
            commandBuffer.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);
        } else
        {
            commandBuffer.draw(mesh->GetVertexCount(), 1, 0, 0);
        }

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
