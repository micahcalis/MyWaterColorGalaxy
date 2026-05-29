#include "Rendering/RenderPasses/Watercolor/OffsetEdgeBlurPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/RenderPasses/Watercolor/GaussianBlurPass.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::Rendering
{
    OffsetEdgeBlurPass::OffsetEdgeBlurPass(GaussDirection blurDirection,
        uint32_t blurDepth,
        float blurSpread,
        float depthThreshold,
        RenderPassEvent event,
        uint32_t offset)
        : blurDirection(blurDirection)
        , blurDepth(blurDepth)
        , blurSpread(blurSpread)
        , depthTreshold(depthThreshold)
        , IRenderPass(std::format("GaussianBlur{}", magic_enum::enum_name(blurDirection)), static_cast<int32_t>(event) + offset)
    {
        blurMaterial = std::make_shared<Rendering::Material>("Galaxy/EdgeBlurBlit");
        blurMaterial->SetInt("_BlurDirection", (int)blurDirection);
        blurMaterial->SetInt("_BlurResolution", (int)blurDepth);
        blurMaterial->SetFloat("_BlurSpread", blurSpread);
        blurMaterial->SetFloat("_DepthThreshold", depthThreshold);
    }

    void OffsetEdgeBlurPass::OnRenderSetup(const RenderContext& context)
    {
        gBufferNormalOffset = context.BlackBox->ReallocateIfNeeded(GBUFFER_NORMAL_OFFSET,
                                                  Core::Screen::Width(),
                                                  Core::Screen::Height(),
                                                  GBUFFER_NORMAL_OFFSET_FORMAT)
                                  .AllocPointer;

        gBufferNormalOffsetPong = context.BlackBox->ReallocateIfNeeded(GBUFFER_NORMAL_OFFSET_PONG,
                                                      Core::Screen::Width(),
                                                      Core::Screen::Height(),
                                                      GBUFFER_NORMAL_OFFSET_FORMAT)
                                      .AllocPointer;

        blurMaterial->SetTexture("_DepthBuffer", context.MainDepthTarget);
    }

    void OffsetEdgeBlurPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        RenderTexture* sourceTexture = blurDirection == GaussDirection::Horizontal
            ? gBufferNormalOffset
            : gBufferNormalOffsetPong;

        commandBuffer->Blit(sourceTexture,
            blurMaterial.get(),
            ShaderPassType::BlitColor,
            context.Output);
    }

    PassDependencyList OffsetEdgeBlurPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(std::string(MAIN_DEPTH),
            ResourceAction::DepthRead,
            ResetOperator::ClearDepth()));

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_NORMAL_OFFSET),
            blurDirection == GaussDirection::Horizontal ? ResourceAction::ColorRead : ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0.5f, 0.5f}),
            static_cast<vk::Format>(GBUFFER_NORMAL_OFFSET_FORMAT)));

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_NORMAL_OFFSET_PONG),
            blurDirection == GaussDirection::Horizontal ? ResourceAction::ColorWrite : ResourceAction::ColorRead,
            ResetOperator::ClearColor({0, 0, 0.5f, 0.5f}),
            static_cast<vk::Format>(GBUFFER_NORMAL_OFFSET_FORMAT)));

        return dependencies;
    }
} // namespace Beer::Rendering
