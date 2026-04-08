#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
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
            System::ContextMask(System::CTXT_GALAXY_BITS),
            System::LayerMask(System::LAYER_UI_BITS));

        Core::DrawCallPool drawPool = context.Register->GetDrawCallPool(drawRequest);
        drawPool.BindDrawCalls();
    }

    PassDependencyList DrawUIPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(MAIN_COLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        return dependencies;
    }
} // namespace Beer::Rendering
