#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "System/Base/Input/ButtonInput.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/PaintTool/ColorMixer/PigmentButton.hpp"

namespace Beer::Rendering
{
    class InjectPaintPass : public IRenderPass
    {
    private:
        WaterColorSimBuffers* simulationBuffers;
        System::Function<System::MouseInput> getMouseInput;
        System::Function<System::UITransform*> getCanvasTransform;
        System::Function<System::PigmentType> getCurrentPigment = nullptr;

        int brushIndex = 0;

    public:
        InjectPaintPass(WaterColorSimBuffers* simulationBuffers,
            System::Function<System::MouseInput> getMouseInput,
            System::Function<System::UITransform*> getCanvasTransform);

        void OnRenderSetup(const RenderContext& context) override;
        void Execute(CommandBuffer* commandBuffer, const RenderContext& context) override;
        PassDependencyList GetDependencies() const override;

        void SetGetCurrentPigment(System::Function<System::PigmentType> getCurrentPigment)
        {
            this->getCurrentPigment = getCurrentPigment;
        }

    private:
        void SetMouseInput() const;
        void SetCanvasRect() const;
    };
} // namespace Beer::Rendering
