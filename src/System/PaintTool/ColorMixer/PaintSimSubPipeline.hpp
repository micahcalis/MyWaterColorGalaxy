#pragma once

#include "Rendering/RenderPasses/Painting/CalculateFluidFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/ClearLiquidsPass.hpp"
#include "Rendering/RenderPasses/Painting/EvaporateWaterPass.hpp"
#include "Rendering/RenderPasses/Painting/InjectPaintPass.hpp"
#include "Rendering/RenderPasses/Painting/RenderPigmentPass.hpp"
#include "Rendering/RenderPasses/Painting/ResolveFluidFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/ResolvePigmentFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/TransferPigmentPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/RenderPasses/Painting/CalculateFluidFluxPass.hpp"
#include "System/Base/Input/ButtonInput.hpp"
#include "System/Components/General/ISubRenderPipeline.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "System/Readback/ImagePixelData.hpp"
#include <memory>

namespace Beer::System
{
    class PaintSimSubPipeline : public ISubRenderPipeline
    {
    private:
        std::unique_ptr<Rendering::WaterColorSimBuffers> simulationBuffers = nullptr;
        std::unique_ptr<Rendering::InjectPaintPass> injectPaintPass = nullptr;
        std::unique_ptr<Rendering::CalculateFluidFluxPass> calculateFluidFluxPass = nullptr;
        std::unique_ptr<Rendering::ResolvePigmentFluxPass> resolvePigmentFluxPass = nullptr;
        std::unique_ptr<Rendering::ResolveFluidFluxPass> resolveFluidFluxPass = nullptr;
        std::unique_ptr<Rendering::TransferPigmentPass> transferPigmentPass = nullptr;
        std::unique_ptr<Rendering::RenderPigmentPass> renderPigmentPass = nullptr;
        std::unique_ptr<Rendering::EvaporateWaterPass> evaporateWaterPass = nullptr;
        std::unique_ptr<Rendering::ClearLiquidsPass> clearLiquidsPass = nullptr;

        Function<ColorPickingState> getColorPickerState = nullptr;
        bool clearMarker = false;

    public:
        PaintSimSubPipeline(Rendering::Material* debugMaterial,
            System::Function<System::MouseInput> getMouseInput,
            System::Function<System::UITransform*> getCanvasTransform,
            System::Function<System::ButtonInput> getDebugButtonInput);

        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;
        [[nodiscard]] Rendering::InjectPaintPass* GetInjectPaintPass() const { return injectPaintPass.get(); }
        void SubscribeToNewCanvasReadback(Function<void, ImagePixelData> readbackFunc);

        void SetGetColorPickerState(Function<ColorPickingState> getColorPickerState)
        {
            this->getColorPickerState = getColorPickerState;
        }

        void MarkClear()
        {
            clearMarker = true;
        }
    };
} // namespace Beer::System
