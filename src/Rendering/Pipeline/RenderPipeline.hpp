#pragma once

#include "CommandBuffer/CommandBuffer.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Frame/FrameBlackbox.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/Frame/FrameGraph.hpp"
#include "Rendering/Pipeline/Frame/FrameBuilder.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
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
        Rendering::PhaseBuffer* transformBuffer = nullptr;
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
        static void BindGlobals(CommandBuffer* commandBuffer, const RenderContext& context);
    };
} // namespace Beer::Rendering
