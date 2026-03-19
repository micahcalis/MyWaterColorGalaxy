#include "Rendering/Pipeline/Frame/FrameGraph.hpp"
#include "FrameGraph.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Pipeline/Frame/Resource/IRenderResource.hpp"

namespace Beer::Rendering
{
    FrameGraph::FrameGraph(std::vector<IRenderPass*> renderPasses)
    {
        for (auto pass : renderPasses)
        {
            renderNodes.push_back(RenderCommandNode(pass));
        }
    }

    void FrameGraph::OnRenderSetup(const RenderContext& context)
    {
        for (auto& node : renderNodes)
        {
            node.RenderPass->OnRenderSetup(context);
        }
    }

    void FrameGraph::PrepareBarriers(const RenderContext& context)
    {
        for (auto& node : renderNodes)
        {
            PassDependencyList deps = node.RenderPass->GetDependencies();

            for (const PassDependency& dep : deps.GetDependencies())
            {
                IRenderResource* resource = context.BlackBox->GetResource<IRenderResource>(dep.GetResourceName());

                if (!resource)
                    continue;

                std::unique_ptr<ISyncBarrier> barrier = resource->GetBarrier(dep.GetAction());
                node.Commands.emplace_back(ResourceActionCommand(std::move(barrier)));
            }
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
