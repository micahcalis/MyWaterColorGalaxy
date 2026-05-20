#include "System/PaintTool/ColorMixer/PaintSimSubPipeline.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Painting/ClearLiquidsPass.hpp"
#include "Rendering/RenderPasses/Painting/EvaporateWaterPass.hpp"
#include "Rendering/RenderPasses/Painting/InjectPaintPass.hpp"
#include "Rendering/RenderPasses/Painting/RenderPigmentPass.hpp"
#include "Rendering/RenderPasses/Painting/ResolveFluidFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/ResolvePigmentFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/TransferPigmentPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/RenderPasses/Painting/CalculateFluidFluxPass.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "System/Readback/ImageReadback.hpp"
#include "System/Readback/ImageReadbackRequest.hpp"
#include <memory>
#include <stdexcept>

namespace Beer::System
{
    PaintSimSubPipeline::PaintSimSubPipeline(Rendering::Material* debugMaterial,
        System::Function<System::MouseInput> getMouseInput,
        System::Function<System::UITransform*> getCanvasTransform,
        System::Function<System::ButtonInput> getDebugButtonInput)
    {
        simulationBuffers = std::make_unique<Rendering::WaterColorSimBuffers>(debugMaterial);

        injectPaintPass = std::make_unique<Rendering::InjectPaintPass>(
            simulationBuffers.get(),
            getMouseInput,
            getCanvasTransform);

        calculateFluidFluxPass = std::make_unique<Rendering::CalculateFluidFluxPass>(
            simulationBuffers.get());

        resolvePigmentFluxPass = std::make_unique<Rendering::ResolvePigmentFluxPass>(
            simulationBuffers.get());

        resolveFluidFluxPass = std::make_unique<Rendering::ResolveFluidFluxPass>(
            simulationBuffers.get());

        transferPigmentPass = std::make_unique<Rendering::TransferPigmentPass>(
            simulationBuffers.get());

        renderPigmentPass = std::make_unique<Rendering::RenderPigmentPass>(
            simulationBuffers.get());

        evaporateWaterPass = std::make_unique<Rendering::EvaporateWaterPass>(
            simulationBuffers.get());

        clearLiquidsPass = std::make_unique<Rendering::ClearLiquidsPass>(
            simulationBuffers.get());
    }

    std::vector<Rendering::IRenderPass*> PaintSimSubPipeline::GetRenderPasses()
    {
        std::vector<Rendering::IRenderPass*> renderPasses{
            calculateFluidFluxPass.get(),
            resolvePigmentFluxPass.get(),
            resolveFluidFluxPass.get(),
            transferPigmentPass.get(),
            renderPigmentPass.get(),
            evaporateWaterPass.get()};

        if (getColorPickerState != nullptr)
        {
            if (getColorPickerState() == ColorPickingState::Idle)
            {
                renderPasses.push_back(injectPaintPass.get());
            }
        }

        if (clearMarker)
        {
            renderPasses.push_back(clearLiquidsPass.get());
            clearMarker = false;
        }

        return renderPasses;
    }

    void PaintSimSubPipeline::SubscribeToNewCanvasReadback(Function<void, ImagePixelData> readbackFunc)
    {
        if (simulationBuffers->PigmentRender == nullptr)
        {
            throw std::runtime_error("Trying to Readback Mixing Canvas with null RenderTexture!");
        }

        if (readbackFunc == nullptr)
        {
            throw std::runtime_error("Trying to Readback Mixing Canvas with null Readback Function!");
        }

        std::unique_ptr<ImageReadbackRequest> readbackRequest = std::make_unique<ImageReadbackRequest>(
            simulationBuffers->PigmentRender);

        System::ImageReadback* readback = static_cast<System::ImageReadback*>(System::IAsyncReadback::Get(std::move(readbackRequest)));
        readback->Subscribe(readbackFunc);
    }
} // namespace Beer::System
