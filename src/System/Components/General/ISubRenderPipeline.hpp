#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include <vector>

namespace Beer::System
{
    class ISubRenderPipeline
    {
    public:
        virtual ~ISubRenderPipeline() = default;
        virtual std::vector<Rendering::IRenderPass*> GetRenderPasses() = 0;
    };
} // namespace Beer::System
