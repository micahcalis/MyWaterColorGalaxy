#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "System/Base/Clock/Clock.hpp"
#include <print>

namespace Beer::Rendering
{
    FullscreenTransitionPass::FullscreenTransitionPass()
        : IRenderPass("Fullscreen Transition", RenderPassEvent::END)
    {
    }

    void FullscreenTransitionPass::OnRenderSetup(const RenderContext& context)
    {
        UpdateFade();
    }

    void FullscreenTransitionPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        if (transitionMaterial == nullptr)
        {
            return;
        }

        commandBuffer->Blit(context.GetMainColorSource(),
            transitionMaterial.get(),
            ShaderPassType::BlitColor,
            context.Output);
    }

    PassDependencyList FullscreenTransitionPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependencies(RenderGlobalUtilities::GetMainColorBlitDependencies());

        return dependencies;
    }

    void FullscreenTransitionPass::UpdateFade()
    {
        if (transitionMaterial == nullptr)
            return;

        float dt = System::Clock::DeltaTime();

        if (state == FadeState::In)
        {
            time += fadeSpeed * dt;
        } else if (state == FadeState::Out)
        {
            time -= fadeSpeed * dt;
        }

        time = std::clamp(time, 0.0f, 1.0f);
        transitionMaterial->SetFloat("_TransitionTime", time);
    }
} // namespace Beer::Rendering
