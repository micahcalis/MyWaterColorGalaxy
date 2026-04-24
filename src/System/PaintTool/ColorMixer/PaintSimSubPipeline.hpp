#pragma once

#include "Rendering/RenderPasses/Painting/CalculateFluidFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/GenerateCanvasPass.hpp"
#include "Rendering/RenderPasses/Painting/InjectPaintPass.hpp"
#include "Rendering/RenderPasses/Painting/ResolveFluidFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/ResolvePigmentFluxPass.hpp"
#include "Rendering/RenderPasses/Painting/TransferPigmentPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/RenderPasses/Painting/CalculateFluidFluxPass.hpp"
#include "System/Base/Input/ButtonInput.hpp"
#include "System/Components/General/ISubRenderPipeline.hpp"
#include <memory>

namespace Beer::System
{
    class PaintSimSubPipeline : public ISubRenderPipeline
    {
    private:
        std::unique_ptr<Rendering::WaterColorSimBuffers> simulationBuffers;
        Rendering::GenerateCanvasPass* generateCanvasPass;
        Rendering::InjectPaintPass* injectPaintPass;
        Rendering::CalculateFluidFluxPass* calculateFluidFluxPass;
        Rendering::ResolvePigmentFluxPass* resolvePigmentFluxPass;
        Rendering::ResolveFluidFluxPass* resolveFluidFluxPass;
        Rendering::TransferPigmentPass* transferPigmentPass;

    public:
        PaintSimSubPipeline(Rendering::Material* debugMaterial,
            System::Function<System::MouseInput> getMouseInput,
            System::Function<System::UITransform*> getCanvasTransform,
            System::Function<System::ButtonInput> getDebugButtonInput);

        std::vector<Rendering::IRenderPass*> GetRenderPasses() const override;
    };
} // namespace Beer::System
