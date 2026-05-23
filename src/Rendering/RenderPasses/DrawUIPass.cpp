#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Drawing/Layer.hpp"

namespace Beer::Rendering
{
    void DrawUIPass::OnRenderSetup(const RenderContext& context)
    {
    }

    void DrawUIPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        System::DrawRequest drawRequest = System::DrawRequest(commandBuffer,
            context,
            Rendering::ShaderPassType::UserInterface,
            System::ContextMask(System::CTXT_PAINT_TOOL_BITS | System::CTXT_GALAXY_UI_BITS),
            System::LayerMask(System::LAYER_UI_BITS));

        Core::DrawCallPool drawPool = context.Register->GetDrawCallPool(drawRequest);
        drawPool.BindDrawCalls();
    }

    PassDependencyList DrawUIPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(VIRTUAL_MAIN_COLOR,
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(PIGMENT_RENDER,
            ResourceAction::ColorRead));

        return dependencies;
    }
} // namespace Beer::Rendering
