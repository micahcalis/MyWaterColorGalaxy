#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "RenderGlobalSettings.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::Rendering
{
    static const float CANGIANTE = 0.2f;
    static const float DILUTION = 0.2f;
    static const float DILUTE_AREA = 0.75f;
    static const float DARK_INTENSITY = 0.5f;
    static const float LIGHT_COL_INTENSITY = 0.5f;
    static const float TURBULENCE_INTENSITY = 0.4f;

    DeferredShadePass::DeferredShadePass()
        : IRenderPass("Deferred Shade", RenderPassEvent::DEFERRED_SHADE)
    {
        blitMaterial = std::make_shared<Material>("DeferredShadeBlit");
        blitMaterial->SetFloat("_Cangiante", CANGIANTE);
        blitMaterial->SetFloat("_Dilution", DILUTION);
        blitMaterial->SetFloat("_DiluteArea", DILUTE_AREA);
        blitMaterial->SetFloat("_DarkIntensity", DARK_INTENSITY);
        blitMaterial->SetFloat("_LightColorIntensity", LIGHT_COL_INTENSITY);
        blitMaterial->SetFloat("_TurbulenceIntensity", TURBULENCE_INTENSITY);
    }

    void DeferredShadePass::OnRenderSetup(const RenderContext& context)
    {
        GBufferAlbedo = context.BlackBox->ReallocateIfNeeded(std::string(GBUFFER_ALBEDO),
                                            Core::Screen::Width(),
                                            Core::Screen::Height(),
                                            GBUFFER_ALBEDO_FORMAT)
                            .AllocPointer;

        GBufferNormal = context.BlackBox->ReallocateIfNeeded(std::string(GBUFFER_NORMAL_OFFSET),
                                            Core::Screen::Width(),
                                            Core::Screen::Height(),
                                            GBUFFER_NORMAL_OFFSET_FORMAT)
                            .AllocPointer;

        GBufferMaterial = context.BlackBox->ReallocateIfNeeded(std::string(GBUFFER_MAT),
                                              Core::Screen::Width(),
                                              Core::Screen::Height(),
                                              GBUFFER_MAT_FORMAT)
                              .AllocPointer;

        GBufferEmission = context.BlackBox->ReallocateIfNeeded(GBUFFER_EMISSION,
                                              Core::Screen::Width(),
                                              Core::Screen::Height(),
                                              GBUFFER_EMISSION_FORMAT)
                              .AllocPointer;

        GBufferWatercolor = context.BlackBox->ReallocateIfNeeded(GBUFFER_WATERCOLOR,
                                                Core::Screen::Width(),
                                                Core::Screen::Height(),
                                                GBUFFER_WATERCOLOR_FORMAT)
                                .AllocPointer;
    }

    void DeferredShadePass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        blitMaterial->SetTexture("_GBufferNormals", GBufferNormal);
        blitMaterial->SetTexture("_GBufferMaterial", GBufferMaterial);
        blitMaterial->SetTexture("_GBufferEmission", GBufferEmission);
        blitMaterial->SetTexture("_GBufferWatercolor", GBufferWatercolor);
        blitMaterial->SetTexture("_DepthBuffer", context.MainDepthTarget);

        commandBuffer->Blit(GBufferAlbedo,
            blitMaterial.get(),
            ShaderPassType::DeferredShade,
            context.Output);
    }

    PassDependencyList DeferredShadePass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
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
