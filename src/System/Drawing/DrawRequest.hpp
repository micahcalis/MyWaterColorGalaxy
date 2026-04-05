#pragma once

#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Shader/FragmentOutput.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Drawing/ContextMask.hpp"
#include "System/Drawing/LayerMask.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct RenderContext;
}

namespace Beer::System
{
    struct DrawRequest
    {
    private:
        Rendering::CommandBuffer* commandBuffer;
        const Rendering::RenderContext& renderContext;
        Rendering::ShaderPassType pass;
        ContextMask contexts;
        LayerMask layers;

    public:
        DrawRequest(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            Rendering::ShaderPassType pass,
            ContextMask contexts,
            LayerMask layers)
            : commandBuffer(commandBuffer), renderContext(renderContext), pass(pass), contexts(contexts), layers(layers)
        {
        }

        Rendering::ShaderPassType GetPass() const { return pass; }

        [[nodiscard]] Rendering::CommandBuffer* GetCommandBuffer() const { return commandBuffer; }
        [[nodiscard]] const Rendering::RenderContext& GetContext() const { return renderContext; }

        [[nodiscard]] bool ValidateContext(ContextType validateContext) const
        {
            return contexts.Has(validateContext);
        }

        [[nodiscard]] bool ValidateLayer(Layer validateLayer) const
        {
            return layers.Has(validateLayer);
        }
    };
} // namespace Beer::System
