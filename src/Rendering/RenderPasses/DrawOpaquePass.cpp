#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "ComputePerlinPass.hpp"
#include "Core/Application/Renderer/DrawCallPool.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Drawing/DrawRequest.hpp"

namespace Beer::Rendering
{
    void DrawOpaquePass::OnRenderSetup(const RenderContext& context)
    {
        context.BlackBox->ReallocateIfNeeded(GBUFFER_ALBEDO,
            Core::Screen::Width(),
            Core::Screen::Height(),
            GBUFFER_ALBEDO_FORMAT);

        context.BlackBox->ReallocateIfNeeded(GBUFFER_NORMAL_OFFSET,
            Core::Screen::Width(),
            Core::Screen::Height(),
            GBUFFER_NORMAL_OFFSET_FORMAT);

        context.BlackBox->ReallocateIfNeeded(GBUFFER_MAT,
            Core::Screen::Width(),
            Core::Screen::Height(),
            GBUFFER_MAT_FORMAT);

        context.BlackBox->ReallocateIfNeeded(GBUFFER_EMISSION,
            Core::Screen::Width(),
            Core::Screen::Height(),
            GBUFFER_EMISSION_FORMAT);

        context.BlackBox->ReallocateIfNeeded(GBUFFER_WATERCOLOR,
            Core::Screen::Width(),
            Core::Screen::Height(),
            GBUFFER_WATERCOLOR_FORMAT);
    }

    void DrawOpaquePass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        System::DrawRequest drawRequest = System::DrawRequest(commandBuffer,
            context,
            Rendering::ShaderPassType::Opaque,
            System::ContextMask(System::CTXT_GALAXY_BITS),
            System::LayerMask(System::LAYER_ALL_BITS));

        Core::DrawCallPool drawPool = context.Register->GetDrawCallPool(drawRequest);
        drawPool.BindDrawCalls();
    }

    PassDependencyList DrawOpaquePass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependencies(RenderGlobalUtilities::GetGBufferDependencies(true));

        dependencies.AddDependency(PassDependency(MAIN_DEPTH,
            ResourceAction::DepthWrite,
            ResetOperator::ClearDepth()));

        dependencies.AddDependency(PassDependency(PERLIN_TEX_NAME,
            ResourceAction::ColorRead));

        return dependencies;
    }
} // namespace Beer::Rendering
