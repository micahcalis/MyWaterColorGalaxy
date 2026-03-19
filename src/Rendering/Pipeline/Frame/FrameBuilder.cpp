#include "Rendering/Pipeline/Frame/FrameBuilder.hpp"
#include <memory>
#include "Dependency/ResourceAction.hpp"
#include "FrameGraph.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include <queue>

namespace Beer::Rendering
{
    FrameGraph FrameBuilder::BuildGraph()
    {
        return FrameGraph(sortedPasses);
    }

    void FrameBuilder::CreateNodeGraph(std::vector<IRenderPass*> passes)
    {
        for (auto pass : passes)
        {
            graph.emplace_back(std::make_unique<RenderPassNode>(pass));
        }
    }

    void FrameBuilder::BuildGraphEdges()
    {
        std::unordered_map<std::string, RenderPassNode*> latestWriters;

        for (auto& node : graph)
        {
            for (const PassDependency& dep : node->Dependencies.GetDependencies())
            {
                const std::string& resourceName = dep.GetResourceName();
                ResourceAction action = dep.GetAction();

                bool isRead = action == ResourceAction::Read;
                bool isWrite = action == ResourceAction::Write;

                RenderPassNode* previousWriter = nullptr;
                auto it = latestWriters.find(resourceName);
                if (it != latestWriters.end())
                {
                    previousWriter = it->second;
                }

                if (previousWriter && previousWriter != node.get() && (isRead || isWrite))
                {
                    previousWriter->AddEdge(node.get());
                    node->InDegree++;
                }

                if (isWrite)
                {
                    latestWriters[resourceName] = node.get();
                }
            }
        }
    }

    std::vector<IRenderPass*> FrameBuilder::TopologicalSort()
    {
        std::vector<IRenderPass*> sortedPasses;
        std::queue<RenderPassNode*> readyQueue;

        for (auto& node : graph)
        {
            if (node->InDegree == 0)
            {
                readyQueue.push(node.get());
            }
        }

        while (!readyQueue.empty())
        {
            RenderPassNode* current = readyQueue.front();
            readyQueue.pop();

            sortedPasses.push_back(current->RenderPass);

            for (RenderPassNode* dependent : current->Edges)
            {
                dependent->InDegree--;

                if (dependent->InDegree == 0)
                {
                    readyQueue.push(dependent);
                }
            }
        }

        if (sortedPasses.size() != graph.size())
        {
            throw std::runtime_error("Frame Graph failed to compile: Cyclic dependency detected!");
        }

        return sortedPasses;
    }
} // namespace Beer::Rendering
