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
        std::unordered_map<std::string, std::vector<RenderPassNode*>> latestReaders;

        for (auto& node : graph)
        {
            for (const PassDependency& dep : node->Dependencies.GetDependencies())
            {
                const std::string& resourceName = dep.GetResourceName();
                ResourceAction action = dep.GetAction();

                bool isRead = IsRead(action);
                bool isWrite = IsWrite(action);

                auto writerIt = latestWriters.find(resourceName);
                if (writerIt != latestWriters.end())
                {
                    RenderPassNode* previousWriter = writerIt->second;
                    if (previousWriter != node.get() && !previousWriter->HasEdgeTo(node.get()))
                    {
                        previousWriter->AddEdge(node.get());
                        node->InDegree++;
                    }
                }

                if (isWrite)
                {
                    auto readersIt = latestReaders.find(resourceName);
                    if (readersIt != latestReaders.end())
                    {
                        for (RenderPassNode* previousReader : readersIt->second)
                        {
                            if (previousReader != node.get() && !previousReader->HasEdgeTo(node.get()))
                            {
                                previousReader->AddEdge(node.get());
                                node->InDegree++;
                            }
                        }
                    }

                    latestWriters[resourceName] = node.get();
                    latestReaders[resourceName].clear();
                } else if (isRead)
                {
                    latestReaders[resourceName].push_back(node.get());
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

    bool FrameBuilder::IsRead(const ResourceAction action) const
    {
        return action == ResourceAction::ColorRead
            || action == ResourceAction::ComputeRead
            || action == ResourceAction::ComputeReadWrite;
    }

    bool FrameBuilder::IsWrite(const ResourceAction action) const
    {
        return action == ResourceAction::ColorWrite
            || action == ResourceAction::DepthWrite
            || action == ResourceAction::ComputeWrite
            || action == ResourceAction::ComputeReadWrite;
    }
} // namespace Beer::Rendering
