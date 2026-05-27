#include "Rendering/RenderPasses/Watercolor/WatercolorPostProcessingPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "DynamicCanvasUpdater.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
#include "WatercolorProcessingBuffers.hpp"
#include <memory>

namespace Beer::Rendering
{
    static const float TREMOR_OFFSET_INTENSITY = 0.005f;
    static const float DEPTH_FADE_THRESHOLD = 20.0f;
    static const float PANNING_PERCENTAGE = 10.0f;
    static const float PAPER_TILING = 2.5f;
    static const float PAPER_DISTORTION_INTENSITY = 0.02f;
    static const float GRANULATION_INTENSITY = 0.25f;
    static const uint32_t SLOPE_KERNEL = 0;

    WatercolorPostProcessingPass::WatercolorPostProcessingPass(WatercolorProcessingBuffers* processingBuffers)
        : processingBuffers(processingBuffers)
        , IRenderPass("Watercolor Post Processing", static_cast<uint32_t>(RenderPassEvent::WATERCOLOR) + 4)
    {
        postProcessingMaterial = std::make_shared<Rendering::Material>("Galaxy/WatercolorProcessing");
        postProcessingMaterial->SetFloat("_TremorOffsetIntensity", TREMOR_OFFSET_INTENSITY);
        postProcessingMaterial->SetFloat("_DepthFadeThreshold", DEPTH_FADE_THRESHOLD);
        postProcessingMaterial->SetFloat("_PaperTiling", PAPER_TILING);
        postProcessingMaterial->SetFloat("_PaperDistortionIntensity", PAPER_DISTORTION_INTENSITY);
        postProcessingMaterial->SetFloat("_GranulationIntensity", GRANULATION_INTENSITY);

        dynamicCanvasUpdater = std::make_unique<DynamicCanvasUpdater>(postProcessingMaterial.get(),
            glm::vec3(0, 0, 1),
            PANNING_PERCENTAGE);

        InitializePaperSlopeMap();
    }

    void WatercolorPostProcessingPass::OnRenderSetup(const RenderContext& context)
    {
        processingBuffers->ReallocateNormalsOffset(context);
        processingBuffers->ReallocateWatercolor(context);
        processingBuffers->ReallocateBlurredColor(context);

        postProcessingMaterial->SetTexture("_NormalsOffset", processingBuffers->GBufferNormalsOffset);
        postProcessingMaterial->SetTexture("_WatercolorData", processingBuffers->GBufferWatercolor);
        postProcessingMaterial->SetTexture("_BlurredColor", processingBuffers->BlurredColor);

        dynamicCanvasUpdater->Update(context.Camera->GetTransform()->GetForward(),
            context.Camera->GetTransform()->GetUp(),
            context.Camera->GetTransform()->GetRight(),
            context.Camera->GetFOV());
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

    void WatercolorPostProcessingPass::InitializePaperSlopeMap()
    {
        paperSourceTex = std::make_shared<Rendering::Texture2D>("Watercolor/Tex_WatercolorPaper");

        slopeContext = std::make_shared<Rendering::ComputeContext>("Texture/ComputeSlopeMap");
        slopeContext->SetTexture("_SourceTex", paperSourceTex.get());
        glm::vec2 texelSize = 1.0f / glm::vec2(paperSourceTex->GetWidth(), paperSourceTex->GetHeight());
        slopeContext->SetVector("_SourceTexelSize", glm::vec4(texelSize, 0, 0));

        TextureMakeSettings makeSettings{};
        makeSettings.Width = paperSourceTex->GetWidth();
        makeSettings.Height = paperSourceTex->GetHeight();
        makeSettings.KernelIndex = SLOPE_KERNEL;

        paperSlopeMap = std::make_shared<Rendering::Texture2D>(Rendering::Texture2D::Make(makeSettings,
            slopeContext.get()));

        postProcessingMaterial->SetTexture("_PaperTex", paperSlopeMap.get());
    }
} // namespace Beer::Rendering
