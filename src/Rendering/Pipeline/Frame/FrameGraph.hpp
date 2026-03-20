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
            : RenderPass(renderPass)
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
        void OnRenderSetup(const RenderContext& context);
        void PrepareBarriers(const RenderContext& context);
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context);

    private:
        RenderingBeginData GetNodeBeginData(RenderCommandNode& node, const RenderContext& context);
    };

} // namespace Beer::Rendering
