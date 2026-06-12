#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"

namespace Beer::Rendering
{
    enum class FadeState
    {
        In,
        Out
    };

    class FullscreenTransitionPass : public IRenderPass
    {
    private:
        std::shared_ptr<Rendering::Material> transitionMaterial = nullptr;
        FadeState state = FadeState::Out;
        float fadeSpeed = 1.0f;
        float time = 0;

    public:
        FullscreenTransitionPass();

        void SetFade(FadeState state, float speed)
        {
            this->state = state;
            fadeSpeed = speed;
        }

        void SetMaterial(std::shared_ptr<Rendering::Material> material)
        {
            transitionMaterial = material;
        }

        bool HasMaterial() const
        {
            return transitionMaterial != nullptr;
        }

        bool BlitsMainTarget() const override
        {
            return true;
        }

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
        void UpdateFade();
    };
} // namespace Beer::Rendering
