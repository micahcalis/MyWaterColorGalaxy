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
} // namespace Beer::Rendering
