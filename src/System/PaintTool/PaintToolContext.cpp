#include "System/PaintTool/PaintToolContext.hpp"
#include "ColorMixer/ColorMixerEntity.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/Painting/InteractivePaintingPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    void PaintToolContext::Load()
    {
        colorMixerEntity = registry.CreateEntity<ColorMixerEntity>();

        interactivePaintingPass = Rendering::IRenderPass::FetchFromRegister<Rendering::InteractivePaintingPass>(
            Rendering::INTERACTIVE_PAINT_PASS, colorMixerEntity->GetColorMixerMat());

        interactivePaintingPass->SetGetCanvasTransform(
            [this]() -> UITransform* { return colorMixerEntity->GetRootTransform(); });

        interactivePaintingPass->SetGetMouseInput(getMouseInput);
        interactivePaintingPass->SetGetDebugKeyInput(getDebugKeyInput);

        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));
    }

    void PaintToolContext::Update()
    {
        IContext::Update();

        if (colorMixerEntity != nullptr)
        {
            colorMixerEntity->Update();
        }
    }

    std::vector<Rendering::IRenderPass*> PaintToolContext::GetRenderPasses()
    {
        return {interactivePaintingPass, drawUIPass};
    }

} // namespace Beer::System
