#pragma once

#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Pipeline/Frame/ResourceActionCommand.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include <vector>

namespace Beer::Rendering
{
    struct RenderCommandNode
    {
    public:
        IRenderPass* RenderPass;
        std::vector<ResourceActionCommand> Commands;

    public:
        RenderCommandNode(IRenderPass* renderPass)
            : RenderPass(renderPass), Commands(renderPass->GetCommands())
        {
        }
    };

    class FrameGraph
    {
    private:
        std::vector<RenderCommandNode> renderNodes;

    public:
        FrameGraph(std::vector<IRenderPass*> renderPasses);
        FrameGraph() {}
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context);
    };

} // namespace Beer::Rendering
