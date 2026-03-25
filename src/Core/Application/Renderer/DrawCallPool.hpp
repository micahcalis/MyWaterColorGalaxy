#pragma once

#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "vulkan/vulkan.hpp"
#include <vector>

namespace Beer::Core
{
    class DrawCallPool
    {
    private:
        Rendering::CommandBuffer* commandBuffer;
        Rendering::ShaderPassType shaderPass;
        std::vector<System::IRenderComponent*> renderComponents;

    public:
        DrawCallPool(Rendering::CommandBuffer* commandBuffer,
            Rendering::ShaderPassType shaderPass,
            std::vector<System::IRenderComponent*> renderComponents)
            : commandBuffer(commandBuffer), shaderPass(shaderPass), renderComponents(std::move(renderComponents))
        {
        }

        void BindDrawCalls();
    };
} // namespace Beer::Core
