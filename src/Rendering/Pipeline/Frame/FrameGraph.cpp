#include "Rendering/Pipeline/Frame/FrameGraph.hpp"
#include "FrameGraph.hpp"

namespace Beer::Rendering
{
    FrameGraph::FrameGraph(std::vector<IRenderPass*> renderPasses)
    {
        for (auto pass : renderPasses)
        {
            renderNodes.push_back(RenderCommandNode(pass));
        }
    }

    void FrameGraph::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        for (auto& node : renderNodes)
        {
            for (auto& resourceCommand : node.Commands)
            {
                resourceCommand.Execute(commandBuffer);
            }

            node.RenderPass->Execute(commandBuffer, context);
        }
    }
} // namespace Beer::Rendering
