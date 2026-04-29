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

namespace Beer::System
{
    PaintSimSubPipeline::PaintSimSubPipeline(Rendering::Material* debugMaterial,
        System::Function<System::MouseInput> getMouseInput,
        System::Function<System::UITransform*> getCanvasTransform,
        System::Function<System::ButtonInput> getDebugButtonInput)
    {
        simulationBuffers = std::make_unique<Rendering::WaterColorSimBuffers>(debugMaterial);

        injectPaintPass = Rendering::IRenderPass::FetchFromRegister<Rendering::InjectPaintPass>(
            "InjectPaintPass",
            simulationBuffers.get(),
            getMouseInput,
            getCanvasTransform);

        calculateFluidFluxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::CalculateFluidFluxPass>(
            "CalculateFluidFluxPass",
            simulationBuffers.get());

        resolvePigmentFluxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::ResolvePigmentFluxPass>(
            "ResolvePigmentFluxPass",
            simulationBuffers.get());

        resolveFluidFluxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::ResolveFluidFluxPass>(
            "ResolveFluidFluxPass",
            simulationBuffers.get());

        transferPigmentPass = Rendering::IRenderPass::FetchFromRegister<Rendering::TransferPigmentPass>(
            "TransferPigmentPass",
            simulationBuffers.get());

        renderPigmentPass = Rendering::IRenderPass::FetchFromRegister<Rendering::RenderPigmentPass>(
            "RenderPigmentPass",
            simulationBuffers.get());

        evaporateWaterPass = Rendering::IRenderPass::FetchFromRegister<Rendering::EvaporateWaterPass>(
            "EvaporateWaterPass",
            simulationBuffers.get());

        clearLiquidsPass = Rendering::IRenderPass::FetchFromRegister<Rendering::ClearLiquidsPass>(
            "ClearLiquidsPass",
            simulationBuffers.get());
    }

    std::vector<Rendering::IRenderPass*> PaintSimSubPipeline::GetRenderPasses()
    {
        std::vector<Rendering::IRenderPass*> renderPasses{injectPaintPass,
            calculateFluidFluxPass,
            resolvePigmentFluxPass,
            resolveFluidFluxPass,
            transferPigmentPass,
            renderPigmentPass,
            evaporateWaterPass};

        if (clearMarker)
        {
            renderPasses.push_back(clearLiquidsPass);
            clearMarker = false;
        }

        return renderPasses;
    }
} // namespace Beer::System
