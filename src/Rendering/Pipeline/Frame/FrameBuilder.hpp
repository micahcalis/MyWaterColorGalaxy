#pragma once

#include "FrameGraph.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include <memory>

namespace Beer::Rendering
{
    struct RenderPassNode
    {
    public:
        IRenderPass* RenderPass;
        PassDependencyList Dependencies;
        int InDegree = 0;
        std::vector<RenderPassNode*> Edges;

    public:
        RenderPassNode(IRenderPass* renderPass)
            : RenderPass(renderPass), Dependencies(renderPass->GetDependencies())
        {
        }

        void AddEdge(RenderPassNode* node)
        {
            Edges.emplace_back(node);
        }
    };

    class FrameBuilder
    {
    private:
        std::vector<IRenderPass*> sortedPasses;
        std::vector<std::unique_ptr<RenderPassNode>> graph;

    public:
        FrameBuilder(std::vector<IRenderPass*> passes)
        {
            CreateNodeGraph(std::move(passes));
            BuildGraphEdges();
            sortedPasses = TopologicalSort();
        }

        FrameBuilder() {}
        FrameGraph BuildGraph();

    private:
        void CreateNodeGraph(std::vector<IRenderPass*> passes);
        void BuildGraphEdges();
        std::vector<IRenderPass*> TopologicalSort();
    };
} // namespace Beer::Rendering
