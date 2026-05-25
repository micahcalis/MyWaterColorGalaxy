#include "Rendering/RenderPasses/Watercolor/GaussianBlurPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include <format>

namespace Beer::Rendering
{
    GaussianBlurPass::GaussianBlurPass(const std::string& sourceName,
        const std::string& destinationName,
        FullscreenResolution destinationResolution,
        GaussDirection blurDirection,
        uint32_t blurDepth,
        float blurSpread,
        RenderPassEvent event,
        uint32_t offset)
        : sourceName(sourceName)
        , destinationName(destinationName)
        , destinationResolution(destinationResolution)
        , blurDirection(blurDirection)
        , blurDepth(blurDepth)
        , blurSpread(blurSpread)
        , IRenderPass(std::format("GaussianBlur{}", magic_enum::enum_name(blurDirection)), static_cast<int32_t>(event) + offset)
    {
        blurMaterial = std::make_shared<Rendering::Material>("Blit/GaussianBlurBlit");
        blurMaterial->SetInt("_BlurDirection", (int)blurDirection);
        blurMaterial->SetInt("_BlurResolution", (int)blurDepth);
        blurMaterial->SetFloat("_BlurSpread", blurSpread);
    }

    void GaussianBlurPass::OnRenderSetup(const RenderContext& context)
    {
        uint32_t resolutionX = (uint32_t)((float)Core::Screen::Width() / (float)destinationResolution);
        uint32_t resolutionY = (uint32_t)((float)Core::Screen::Height() / (float)destinationResolution);

        sourceTexture = context.BlackBox->ReallocateIfNeeded(sourceName,
                                            resolutionX,
                                            resolutionY,
                                            Core::Screen::ColorFormat())
                            .AllocPointer;

        context.BlackBox->ReallocateIfNeeded(destinationName,
            resolutionX,
            resolutionY,
            Core::Screen::ColorFormat());

        blurMaterial->SetVector("_SourceTexelSize",
            glm::vec4(1.0f / static_cast<float>(resolutionX),
                1.0f / static_cast<float>(resolutionY),
                0,
                0));
    }

    void GaussianBlurPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(sourceTexture,
            blurMaterial.get(),
            ShaderPassType::BlitColor,
            context.Output);
    }

    PassDependencyList GaussianBlurPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(sourceName,
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(destinationName,
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        return dependencies;
    }
} // namespace Beer::Rendering
