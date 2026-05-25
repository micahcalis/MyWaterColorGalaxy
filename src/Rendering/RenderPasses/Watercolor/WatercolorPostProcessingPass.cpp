#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "WatercolorProcessingBuffers.hpp"

namespace Beer::Rendering
{
    static const float TREMOR_OFFSET_INTENSITY = 0.005f;
    static const float DEPTH_FADE_THRESHOLD = 20.0f;

    WatercolorPostProcessingPass::WatercolorPostProcessingPass(WatercolorProcessingBuffers* processingBuffers)
        : processingBuffers(processingBuffers)
        , IRenderPass("Watercolor Post Processing", static_cast<uint32_t>(RenderPassEvent::WATERCOLOR) + 4)
    {
        postProcessingMaterial = std::make_shared<Rendering::Material>("Galaxy/WatercolorProcessing");
        postProcessingMaterial->SetFloat("_TremorOffsetIntensity", TREMOR_OFFSET_INTENSITY);
        postProcessingMaterial->SetFloat("_DepthFadeThreshold", DEPTH_FADE_THRESHOLD);
    }

    void WatercolorPostProcessingPass::OnRenderSetup(const RenderContext& context)
    {
        processingBuffers->ReallocateNormalsOffset(context);
        processingBuffers->ReallocateWatercolor(context);
        processingBuffers->ReallocateBlurredColor(context);

        postProcessingMaterial->SetTexture("_NormalsOffset", processingBuffers->GBufferNormalsOffset);
        postProcessingMaterial->SetTexture("_WatercolorData", processingBuffers->GBufferWatercolor);
        postProcessingMaterial->SetTexture("_BlurredColor", processingBuffers->BlurredColor);
    }

    void WatercolorPostProcessingPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(context.GetMainColorSource(),
            postProcessingMaterial.get(),
            ShaderPassType::WatercolorProcessing,
            context.Output);
    }

    PassDependencyList WatercolorPostProcessingPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependencies(RenderGlobalUtilities::GetMainColorBlitDependencies());

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

        dependencies.AddDependency(PassDependency(WC_COLORBLIT_TEX_A,
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        return dependencies;
    }
} // namespace Beer::Rendering
