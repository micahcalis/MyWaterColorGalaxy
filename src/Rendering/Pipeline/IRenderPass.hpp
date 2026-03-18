#pragma once

#include "Frame/ResourceActionCommand.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"

namespace Beer::Rendering
{
    class IRenderPass
    {
    public:
        virtual ~IRenderPass() = default;
        virtual void Execute(CommandBuffer& commandBuffer) = 0;
        virtual PassDependencyList GetDependencies() const = 0;
        virtual std::vector<ResourceActionCommand> GetCommands() const = 0;
    };
} // namespace Beer::Rendering
