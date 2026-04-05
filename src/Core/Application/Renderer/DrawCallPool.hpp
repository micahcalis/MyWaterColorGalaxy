#pragma once

#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Shader/FragmentOutput.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "vulkan/vulkan.hpp"
#include <vector>

namespace Beer::Rendering
{
    struct RenderContext;
}

namespace Beer::Core
{
    class DrawCallPool
    {
    private:
        Rendering::CommandBuffer* commandBuffer;
        const Rendering::RenderContext& renderContext;
        Rendering::ShaderPassType shaderPass;
        std::vector<System::IRenderComponent*> renderComponents;

    public:
        DrawCallPool(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            Rendering::ShaderPassType shaderPass,
            std::vector<System::IRenderComponent*> renderComponents)
            : commandBuffer(commandBuffer), renderContext(renderContext), shaderPass(shaderPass), renderComponents(std::move(renderComponents))
        {
        }

        void BindDrawCalls();
    };
} // namespace Beer::Core
