#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::Rendering
{
    void DeferredShadePass::OnRenderSetup(const RenderContext& context)
    {
        GBufferAlbedo = context.BlackBox->ReallocateIfNeeded(std::string(GBUFFER_ALBEDO),
                                            Core::Screen::Width(),
                                            Core::Screen::Height(),
                                            Core::Screen::ColorFormat())
                            .AllocPointer;

        GBufferNormal = context.BlackBox->ReallocateIfNeeded(std::string(GBUFFER_NORMAL),
                                            Core::Screen::Width(),
                                            Core::Screen::Height(),
                                            GBUFFER_NORMAL_FORMAT)
                            .AllocPointer;

        GBufferMaterial = context.BlackBox->ReallocateIfNeeded(std::string(GBUFFER_MAT),
                                              Core::Screen::Width(),
                                              Core::Screen::Height(),
                                              Core::Screen::ColorFormat())
                              .AllocPointer;
    }

    void DeferredShadePass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        blitMaterial->SetTexture("_GBufferNormals", GBufferNormal);
        blitMaterial->SetTexture("_GBufferMaterial", GBufferMaterial);
        blitMaterial->SetTexture("_DepthBuffer", context.MainDepthTarget);

        commandBuffer->Blit(GBufferAlbedo,
            blitMaterial.get(),
            ShaderPassType::DeferredShade,
            context.Output);
    }

    PassDependencyList DeferredShadePass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(MAIN_COLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(std::string(MAIN_DEPTH),
            ResourceAction::DepthRead,
            ResetOperator::ClearDepth()));

        dependencies.AddDependencies(RenderGlobalUtilities::GetGBufferDependencies(false));

        return dependencies;
    }
} // namespace Beer::Rendering
