#pragma once

#include "System/Drawing/ShaderPassMask.hpp"
#include "System/Drawing/ContextMask.hpp"
#include "System/Drawing/LayerMask.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::System
{
    struct DrawRequest
    {
    private:
        vk::CommandBuffer commandBuffer;
        ShaderPassMask passes;
        ContextMask contexts;
        LayerMask layers;

    public:
        DrawRequest(ShaderPassMask passes,
            ContextMask contexts,
            LayerMask layers)
            : passes(passes), contexts(contexts), layers(layers)
        {
        }

        [[nodiscard]] vk::CommandBuffer GetCommandBuffer() const { return commandBuffer; }

        [[nodiscard]] bool ValidatePass(Rendering::ShaderPassType validatePass) const
        {
            return passes.Has(validatePass);
        }

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
