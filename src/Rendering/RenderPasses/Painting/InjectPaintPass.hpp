#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Painting/GenerateCanvasPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "System/Base/Input/MouseInput.hpp"

namespace Beer::Rendering
{
    class InjectPaintPass : public IRenderPass
    {
    private:
        WaterColorSimBuffers* simulationBuffers;
        System::Function<System::MouseInput> getMouseInput;
        System::Function<System::UITransform*> getCanvasTransform;

    public:
        InjectPaintPass(WaterColorSimBuffers* simulationBuffers,
            System::Function<System::MouseInput> getMouseInput,
            System::Function<System::UITransform*> getCanvasTransform);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

    private:
        void SetMouseInput() const;
        void SetCanvasRect() const;
    };
} // namespace Beer::Rendering
