#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"

namespace Beer::Rendering
{
    void DrawOpaquePass::OnRenderSetup(const RenderContext& context)
    {
        colorTarget = context.BlackBox->ReallocateIfNeeded(std::string(MAIN_COLOR),
            Core::Screen::Width(),
            Core::Screen::Height(),
            Core::Screen::ColorFormat());
    }

    void DrawOpaquePass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        // do draw stuff (ignore for now)
    }

    PassDependencyList DrawOpaquePass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(MAIN_COLOR),
            ResourceAction::ColorWrite));

        return dependencies;
    }
} // namespace Beer::Rendering
