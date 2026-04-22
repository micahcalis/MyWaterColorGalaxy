#include "System/PaintTool/ColorMixer/PaintSimSubPipeline.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Painting/GenerateCanvasPass.hpp"
#include "Rendering/RenderPasses/Painting/InjectPaintPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "System/Drawing/RenderRegister.hpp"

namespace Beer::System
{
    PaintSimSubPipeline::PaintSimSubPipeline(Rendering::Material* debugMaterial,
        System::Function<System::MouseInput> getMouseInput,
        System::Function<System::UITransform*> getCanvasTransform)
    {
        simulationBuffers = std::make_unique<Rendering::WaterColorSimBuffers>(debugMaterial);

        generateCanvasPass = Rendering::IRenderPass::FetchFromRegister<Rendering::GenerateCanvasPass>(
            "GenerateCanvasPass",
            simulationBuffers.get());

        injectPaintPass = Rendering::IRenderPass::FetchFromRegister<Rendering::InjectPaintPass>(
            "InjectPaintPass",
            simulationBuffers.get(),
            getMouseInput,
            getCanvasTransform);
    }

    std::vector<Rendering::IRenderPass*> PaintSimSubPipeline::GetRenderPasses() const
    {
        return {generateCanvasPass, injectPaintPass};
    }
} // namespace Beer::System
