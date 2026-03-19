#include "Rendering/Pipeline/RenderPipeline.hpp"
#include "CommandBuffer/RenderContext.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Light/ILight.hpp"
#include <memory>

namespace Beer::Rendering
{
    RenderPipeline::RenderPipeline(const Core::Device* device, Core::UploadManager* uploadManager)
    {
        frameBlackbox = std::make_unique<FrameBlackbox>(device, uploadManager);
    }

    void RenderPipeline::InitializeFrame()
    {
        frameBuilder = FrameBuilder(GetRenderPasses());
        frameGraph = frameBuilder.BuildGraph();

        RenderContext context = GetRenderContext();
        frameGraph.OnRenderSetup(context);
    }

    void RenderPipeline::ExecuteFrame(CommandBuffer* commandBuffer)
    {
        RenderContext context = GetRenderContext();
        frameGraph.Execute(commandBuffer, context);
    }

    std::vector<IRenderPass*> RenderPipeline::GetRenderPasses()
    {
        return {drawOpaquePass.get()};
    }

    RenderContext RenderPipeline::GetRenderContext()
    {
        RenderContext context{};
        context.BlackBox = frameBlackbox.get();
        context.Camera = System::Camera::Main();
        context.MainLight = System::ILight::Main();
        return context;
    }
} // namespace Beer::Rendering
