#include "Rendering/Pipeline/RenderPipeline.hpp"
#include "CommandBuffer/RenderContext.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Light/ILight.hpp"
#include <memory>

namespace Beer::Rendering
{
    RenderPipeline::RenderPipeline(const Core::Device* device, Core::UploadManager* uploadManager)
    {
        frameBlackbox = std::make_unique<FrameBlackbox>(device, uploadManager);

        colorTarget = frameBlackbox->CreateRenderTexture2D(std::string(Rendering::MAIN_COLOR),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::ColorFormat());

        depthTarget = frameBlackbox->CreateRenderTexture2D(std::string(Rendering::MAIN_DEPTH),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::DepthFormat());
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

    void RenderPipeline::Present()
    {
        // TODO: blit main render tex to swapchain image
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
        context.MainColorTarget = colorTarget;
        context.MainDepthTarget = depthTarget;
        return context;
    }
} // namespace Beer::Rendering
