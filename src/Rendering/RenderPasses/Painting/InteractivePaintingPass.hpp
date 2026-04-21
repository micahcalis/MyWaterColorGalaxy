#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Base/Input/ButtonInput.hpp"

namespace Beer::Rendering
{
    static const std::string INTERACTIVE_PAINT_NAME = "InteractivePaintBuffer";
    static const uint32_t I_PAINT_RES_X = 512;
    static const uint32_t I_PAINT_RES_Y = 512;

    class InteractivePaintingPass : public IRenderPass
    {
    private:
        System::Function<System::MouseInput> getMouseInput;
        System::Function<System::UITransform*> getCanvasTransform;
        System::Function<System::ButtonInput> getDebugKeyInput;
        std::shared_ptr<ComputeContext> paintingCompContext;
        uint32_t paintingKernel;
        Material* displayMaterial;

    public:
        InteractivePaintingPass(Material* displayMaterial);

        void SetGetMouseInput(System::Function<System::MouseInput> getMouseInput)
        {
            this->getMouseInput = getMouseInput;
        }

        void SetGetCanvasTransform(System::Function<System::UITransform*> getCanvasTransform)
        {
            this->getCanvasTransform = getCanvasTransform;
        }

        void SetGetDebugKeyInput(System::Function<System::ButtonInput> getDebugKeyInput)
        {
            this->getDebugKeyInput = getDebugKeyInput;
        }

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

    private:
        void SetPaintingParams(RenderTexture* paintTexture);
        void UpdateRandomColor();
    };
} // namespace Beer::Rendering
