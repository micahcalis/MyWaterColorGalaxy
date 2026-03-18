#pragma once

#include "Frame/FrameGraph.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"

namespace Beer::Rendering
{
    class IRenderPipeline
    {
    private:
        FrameGraph frameGraph;

    public:
        void InitializeFrame(); // build frame graph
        void ExecuteFrame();    // render frame graph
        void Cleanup();         // cleanup frame graph and resources

    private:
        std::vector<IRenderPass> GetRenderPasses();
    };
} // namespace Beer::Rendering
