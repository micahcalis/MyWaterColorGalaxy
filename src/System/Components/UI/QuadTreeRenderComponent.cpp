#include "System/Components/UI/QuadTreeRenderComponent.hpp"
#include "Rendering/Quads/QuadDrawCallPool.hpp"
#include <memory>
#include <stdexcept>

namespace Beer::System
{
    QuadTreeRenderComponent::QuadTreeRenderComponent()
    {
        quadBuffer = std::make_unique<Rendering::QuadBuffer>();
        drawCallPool = std::make_unique<Rendering::QuadDrawCallPool>(quadBuffer.get());
    }

    void QuadTreeRenderComponent::UpdateQuadDraw()
    {
        if (getRenderItems == nullptr)
        {
            throw std::runtime_error("GetRenderItems() not set on RenderComponents");
        }

        drawCallPool->Update(getRenderItems());
    }

    BindHistory QuadTreeRenderComponent::Bind(BindMask mask,
        Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& renderContext,
        const Rendering::ShaderPassType pass)
    {
        commandBuffer->BindQuadBuffer(quadBuffer.get());

        for (auto& drawCall : drawCallPool->GetDrawCalls())
        {
            drawCall.Execute(commandBuffer, renderContext, pass);

            if (drawCall.IsText())
            {
                commandBuffer->BindQuadBuffer(quadBuffer.get());
            }
        }

        return BindHistory(nullptr, nullptr, nullptr);
    }

    BindMask QuadTreeRenderComponent::GetBindMask(const BindHistory& history) const
    {
        uint32_t bindMaskBits = 0;
        bindMaskBits |= BIND_SHADER_BITS;
        bindMaskBits |= BIND_MATERIAL_BITS;
        bindMaskBits |= BIND_MESH_BITS;

        return BindMask(bindMaskBits);
    }
} // namespace Beer::System
