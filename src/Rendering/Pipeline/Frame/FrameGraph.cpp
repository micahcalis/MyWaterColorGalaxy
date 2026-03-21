#include "Rendering/Pipeline/Frame/FrameGraph.hpp"
#include "FrameGraph.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderingBeginData.hpp"
#include "Rendering/Pipeline/Frame/Resource/IRenderResource.hpp"
#include <print>

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
        commandBuffer->Begin();

        Rendering::Shader::Globals()->Bind(commandBuffer->GetVk());

        for (auto& node : renderNodes)
        {
            for (auto& resourceCommand : node.Commands)
            {
                resourceCommand.Execute(commandBuffer);
            }

            RenderingBeginData beginData = GetNodeBeginData(node, context);
            commandBuffer->BeginRendering(beginData);

            node.RenderPass->Execute(commandBuffer, context);
            commandBuffer->EndRendering();
        }
    }

    RenderingBeginData FrameGraph::GetNodeBeginData(RenderCommandNode& node, const RenderContext& context)
    {
        RenderingBeginData beginData{};
        beginData.WritesToDepth = false;

        bool extentSet = false;

        PassDependencyList deps = node.RenderPass->GetDependencies();

        for (const PassDependency& dep : deps.GetDependencies())
        {
            if (dep.GetAction() == ResourceAction::ColorWrite || dep.GetAction() == ResourceAction::DepthWrite)
            {
                RenderTexture* texture = context.BlackBox->GetResource<RenderTexture>(dep.GetResourceName());

                if (!texture)
                    continue;

                if (!extentSet)
                {
                    beginData.Width = texture->Width();
                    beginData.Height = texture->Height();
                    extentSet = true;
                }

                bool isDepth = false;
                vk::RenderingAttachmentInfo info = texture->GetAttachmentInfo(dep.GetResetOperator(), isDepth);

                if (dep.GetAction() == ResourceAction::ColorWrite)
                {
                    beginData.ColorWriteTargets.push_back(info);
                } else if (dep.GetAction() == ResourceAction::DepthWrite)
                {
                    beginData.DepthWriteTarget = info;
                    beginData.WritesToDepth = true;
                }
            }
        }

        return beginData;
    }

    static std::string ActionToString(ResourceAction action)
    {
        switch (action)
        {
        case ResourceAction::Read: return "Read";
        case ResourceAction::ColorWrite: return "ColorWrite";
        case ResourceAction::DepthWrite: return "DepthWrite";
        default: return "Unknown";
        }
    }

    void FrameGraph::PrintGraph() const
    {
        std::println("--- Frame Graph Execution ---");

        for (size_t i = 0; i < renderNodes.size(); ++i)
        {
            const RenderCommandNode& node = renderNodes[i];
            PassDependencyList deps = node.RenderPass->GetDependencies();

            std::println("[{}] Pass: {}", i, deps.GetPassName());
            std::println("    Barriers: {}", node.Commands.size());

            for (const PassDependency& dep : deps.GetDependencies())
            {
                std::println("    -> Resource: '{}' | Action: {}",
                    dep.GetResourceName(),
                    ActionToString(dep.GetAction()));
            }
        }
        std::println("-----------------------------");
    }
} // namespace Beer::Rendering
