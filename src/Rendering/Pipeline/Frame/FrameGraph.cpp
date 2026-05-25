#include "Rendering/Pipeline/Frame/FrameGraph.hpp"
#include "Dependency/ResetOperator.hpp"
#include "FrameGraph.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderingBeginData.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/Frame/Resource/IRenderResource.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/SyncTargetState.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Delegates/Delegate.hpp"
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
        context.ResetMainColorPong();

        for (auto& node : renderNodes)
        {
            PassDependencyList deps = node.RenderPass->GetDependencies();

            if (node.RenderPass->BlitsMainTarget())
            {
                context.RegisterMainColorPongPass();
            }

            for (const PassDependency& dep : deps.GetDependencies())
            {
                const std::string* resourceNamePtr = &dep.GetResourceName();

                if (*resourceNamePtr == VIRTUAL_MAIN_COLOR)
                {
                    if (dep.GetAction() == ResourceAction::ColorRead)
                    {
                        resourceNamePtr = node.RenderPass->BlitsMainTarget()
                            ? &context.GetMainColorSourceName()
                            : &context.GetMainColorDestinationName();
                    } else if (dep.GetAction() == ResourceAction::ColorWrite)
                    {
                        resourceNamePtr = &context.GetMainColorDestinationName();
                    }
                }

                const std::string& resourceName = *resourceNamePtr;
                IRenderResource* resource = context.BlackBox->GetResource<IRenderResource>(resourceName);

                if (!resource)
                    continue;

                std::unique_ptr<ISyncBarrier> barrier = resource->GetBarrier(dep.GetAction());

                node.Commands.emplace_back(ResourceActionCommand(std::move(barrier)));
            }
        }
    }

    void FrameGraph::Execute(CommandBuffer* commandBuffer,
        RenderContext& context,
        System::Function<void> bindGlobals)
    {
        commandBuffer->Begin();
        bindGlobals();
        context.ResetMainColorPong();

        std::unordered_set<std::string> clearedResources;

        for (auto& node : renderNodes)
        {
            if (node.RenderPass->BlitsMainTarget())
            {
                context.RegisterMainColorPongPass();
            }

            for (auto& resourceCommand : node.Commands)
            {
                resourceCommand.Execute(commandBuffer);
            }

            RenderingBeginData beginData = GetNodeBeginData(node, context, clearedResources);
            commandBuffer->BeginRendering(beginData);

            node.RenderPass->Execute(commandBuffer, context);
            commandBuffer->EndRendering(beginData.IsDrawPass);
        }
    }

    RenderingBeginData FrameGraph::GetNodeBeginData(RenderCommandNode& node,
        RenderContext& context,
        std::unordered_set<std::string>& clearedResources)
    {
        RenderingBeginData beginData{};
        beginData.IsDrawPass = false;

        bool extentSet = false;

        PassDependencyList deps = node.RenderPass->GetDependencies();
        context.Output = deps.GetOutput();

        for (const PassDependency& dep : deps.GetDependencies())
        {
            if (dep.GetAction() == ResourceAction::ColorWrite
                || dep.GetAction() == ResourceAction::DepthWrite
                || dep.GetAction() == ResourceAction::DephTestOnly)
            {
                const std::string* resourceNamePtr = &dep.GetResourceName();

                if (*resourceNamePtr == VIRTUAL_MAIN_COLOR)
                {
                    resourceNamePtr = &context.GetMainColorDestinationName();
                }

                const std::string& resourceName = *resourceNamePtr;
                RenderTexture* texture = context.BlackBox->GetResource<RenderTexture>(resourceName);

                if (!texture)
                    continue;

                if (!extentSet)
                {
                    beginData.Width = texture->Width();
                    beginData.Height = texture->Height();
                    extentSet = true;
                }

                bool isDepth = false;
                ResetOperator resetOperator = dep.GetResetOperator();

                if (clearedResources.find(resourceName) == clearedResources.end())
                {
                    clearedResources.insert(resourceName);
                } else
                {
                    resetOperator.LoadOp = vk::AttachmentLoadOp::eLoad;
                }

                vk::ImageLayout targetLayout = SyncTargetState::GetState(dep.GetAction()).Layout;
                vk::RenderingAttachmentInfo info = texture->GetAttachmentInfo(targetLayout, resetOperator, isDepth);

                if (dep.GetAction() == ResourceAction::ColorWrite)
                {
                    beginData.ColorWriteTargets.push_back(info);
                    beginData.IsDrawPass = true;
                } else if (dep.GetAction() == ResourceAction::DepthWrite || dep.GetAction() == ResourceAction::DephTestOnly)
                {
                    beginData.DepthWriteTarget = info;
                    beginData.TestsDepth = true;
                    beginData.IsDrawPass = true;
                }
            }
        }

        return beginData;
    }

    static std::string ActionToString(ResourceAction action)
    {
        switch (action)
        {
        case ResourceAction::ColorRead: return "ColorRead";
        case ResourceAction::ColorWrite: return "ColorWrite";
        case ResourceAction::DepthWrite: return "DepthWrite";
        case ResourceAction::ComputeRead: return "ComputeRead";
        case ResourceAction::ComputeWrite: return "ComputeWrite";
        case ResourceAction::ComputeReadWrite: return "ComputeReadWrite";
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
