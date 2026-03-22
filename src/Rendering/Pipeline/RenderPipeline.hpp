#pragma once

#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Frame/FrameBlackbox.hpp"
#include "Rendering/Pipeline/Frame/FrameGraph.hpp"
#include "Rendering/Pipeline/Frame/FrameBuilder.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "System/Drawing/RenderRegister.hpp"

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
        System::RenderRegister* renderRegister = nullptr;

    public:
        RenderPipeline(const Core::Device* device,
            Core::UploadManager* uploadManager,
            System::RenderRegister* renderRegister);

        void InitializeFrame();
        void ExecuteFrame(CommandBuffer* commandBuffer);
        void FinalBlit(CommandBuffer* commandBuffer, vk::Image swapchainImage, vk::Extent2D swapchainExtent);
        [[nodiscard]] FrameBlackbox* GetBlackbox() const { return frameBlackbox.get(); }

    private:
        std::vector<IRenderPass*> GetSortedRenderPasses();
        RenderContext GetRenderContext();
    };
} // namespace Beer::Rendering
