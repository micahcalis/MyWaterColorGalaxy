#pragma once

#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include <string>

namespace Beer::Rendering
{
    class ResourceActionCommand
    {
    private:
        std::string resourceName;
        ResourceAction action;

    public:
        void Execute(CommandBuffer* commandBuffer)
        {
        }
    };
} // namespace Beer::Rendering
