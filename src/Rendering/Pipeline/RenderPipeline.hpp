#pragma once

#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Frame/FrameBlackbox.hpp"
#include "Rendering/Pipeline/Frame/FrameGraph.hpp"
#include "Rendering/Pipeline/Frame/FrameBuilder.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"

namespace Beer::Rendering
{
    class RenderPipeline
    {
    private:
        std::unique_ptr<FrameBlackbox> frameBlackbox = nullptr;
        FrameBuilder frameBuilder{};
        FrameGraph frameGraph{};

        Rendering::RenderTexture* colorTarget = nullptr;
        Rendering::RenderTexture* depthTarget = nullptr;

        // hardcoded for now, render passes should be selected from contexts
        std::unique_ptr<DrawOpaquePass> drawOpaquePass = nullptr;

    public:
        RenderPipeline(const Core::Device* device, Core::UploadManager* uploadManager);
        void InitializeFrame();
        void ExecuteFrame(CommandBuffer* commandBuffer);
        void Present();
        [[nodiscard]] FrameBlackbox* GetBlackbox() const { return frameBlackbox.get(); }

    private:
        std::vector<IRenderPass*> GetRenderPasses();
        RenderContext GetRenderContext();
    };
} // namespace Beer::Rendering
