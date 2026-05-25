#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::Rendering
{
    static const float TREMOR_OFFSET_INTENSITY = 0.005f;
    static const float DEPTH_FADE_THRESHOLD = 20.0f;

    WatercolorPostProcessingPass::WatercolorPostProcessingPass(WatercolorProcessingBuffers* processingBuffers)
        : processingBuffers(processingBuffers)
        , IRenderPass("Watercolor Post Processing", RenderPassEvent::WATERCOLOR)
    {
        postProcessingMaterial = std::make_shared<Rendering::Material>("Galaxy/WatercolorProcessing");
        postProcessingMaterial->SetFloat("_TremorOffsetIntensity", TREMOR_OFFSET_INTENSITY);
        postProcessingMaterial->SetFloat("_DepthFadeThreshold", DEPTH_FADE_THRESHOLD);
    }

    void WatercolorPostProcessingPass::OnRenderSetup(const RenderContext& context)
    {
        processingBuffers->ReallocateNormalsOffset(context);
        processingBuffers->ReallocateWatercolor(context);
    }

    void WatercolorPostProcessingPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        postProcessingMaterial->SetTexture("_NormalsOffset", processingBuffers->GBufferNormalsOffset);
        postProcessingMaterial->SetTexture("_WatercolorData", processingBuffers->GBufferWatercolor);

        commandBuffer->Blit(context.GetMainColorSource(),
            postProcessingMaterial.get(),
            ShaderPassType::WatercolorProcessing,
            context.Output);
    }

    PassDependencyList WatercolorPostProcessingPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(std::string(MAIN_DEPTH),
            ResourceAction::DepthRead,
            ResetOperator::ClearDepth()));

        dependencies.AddDependency(PassDependency(GBUFFER_NORMAL_OFFSET,
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(GBUFFER_NORMAL_OFFSET_FORMAT)));

        dependencies.AddDependency(PassDependency(GBUFFER_WATERCOLOR,
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(GBUFFER_WATERCOLOR_FORMAT)));

        return dependencies;
    }
} // namespace Beer::Rendering
