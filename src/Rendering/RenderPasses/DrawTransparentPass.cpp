#include "Rendering/RenderPasses/DrawTransparentPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Drawing/Layer.hpp"

namespace Beer::Rendering
{
    void DrawTransparentPass::OnRenderSetup(const RenderContext& context)
    {
    }

    void DrawTransparentPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        System::DrawRequest drawRequest = System::DrawRequest(commandBuffer,
            context,
            Rendering::ShaderPassType::Transparent,
            System::ContextMask(System::CTXT_GALAXY_BITS),
            System::LayerMask(System::LAYER_ALL_BITS));

        Core::DrawCallPool drawPool = context.Register->GetDrawCallPool(drawRequest);
        drawPool.BindDrawCalls();
    }

    PassDependencyList DrawTransparentPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(std::string(MAIN_DEPTH),
            ResourceAction::DephTestOnly,
            ResetOperator::ClearDepth()));

        return dependencies;
    }

} // namespace Beer::Rendering
