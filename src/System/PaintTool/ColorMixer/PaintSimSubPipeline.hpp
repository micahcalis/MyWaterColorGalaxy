#pragma once

#include "Rendering/RenderPasses/Painting/GenerateCanvasPass.hpp"
#include "Rendering/RenderPasses/Painting/InjectPaintPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/RenderPasses/Painting/WaterDynamicsPass.hpp"
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
        Rendering::WaterDynamicsPass* waterDynamicsPass;

    public:
        PaintSimSubPipeline(Rendering::Material* debugMaterial,
            System::Function<System::MouseInput> getMouseInput,
            System::Function<System::UITransform*> getCanvasTransform);

        std::vector<Rendering::IRenderPass*> GetRenderPasses() const override;
    };
} // namespace Beer::System
