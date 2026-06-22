#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Texture/Texture2D.hpp"

namespace Beer::Rendering
{
    using TransitionInitialization = System::Function<std::shared_ptr<Rendering::Material>,
        std::shared_ptr<Rendering::ComputeContext>&,
        std::shared_ptr<Rendering::Texture2D>&>;

    enum class FadeState : int
    {
        In = 0,
        Out = 1
    };

    class FullscreenTransitionPass : public IRenderPass
    {
    private:
        std::shared_ptr<Rendering::Material> transitionMaterial = nullptr;
        FadeState state = FadeState::Out;
        float fadeSpeed = 1.0f;
        float time = 0;
        std::shared_ptr<Rendering::ComputeContext> transitionContext = nullptr;
        std::shared_ptr<Rendering::Texture2D> transitionTexture = nullptr;

    public:
        FullscreenTransitionPass();

        void SetFade(FadeState state, float speed)
        {
            this->state = state;
            fadeSpeed = speed;
            transitionMaterial->SetInt("_FadeState", static_cast<int>(state));
        }

        void Initialize(TransitionInitialization initializationFunc)
        {
            transitionMaterial = initializationFunc(transitionContext, transitionTexture);
        }

        bool HasMaterial() const
        {
            return transitionMaterial != nullptr;
        }

        bool BlitsMainTarget() const override
        {
            return true;
        }

        Material* GetMaterial() const { return transitionMaterial.get(); }

    private:
        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;
        void UpdateFade();
    };
} // namespace Beer::Rendering
