#pragma once

#include "CommandBuffer/RenderContext.hpp"
#include "Frame/ResourceActionCommand.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"

namespace Beer::Rendering
{
    class IRenderPass
    {
    protected:
        std::string name;

    public:
        virtual ~IRenderPass() = default;

        IRenderPass(std::string name)
            : name(name)
        {
        }

        virtual void OnRenderSetup(const RenderContext& context) = 0;
        virtual void Execute(CommandBuffer* commandBuffer, const RenderContext& context) = 0;
        virtual PassDependencyList GetDependencies() const = 0;
    };
} // namespace Beer::Rendering
