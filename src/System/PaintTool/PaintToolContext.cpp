#include "System/PaintTool/PaintToolContext.hpp"
#include "ColorMixer/ColorMixerEntity.hpp"
#include "ColorMixer/PaintSimSubPipeline.hpp"
#include "ColorMixer/PigmentButton.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    void PaintToolContext::Load()
    {
        colorMixerEntity = registry.CreateEntity<ColorMixerEntity>();

        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        paintSimSubPipeline = std::make_unique<PaintSimSubPipeline>(
            colorMixerEntity->GetColorMixerMat(),
            getMouseInput,
            [this]() -> UITransform* { return colorMixerEntity->GetRootTransform(); },
            getDebugKeyInput);

        paintSimSubPipeline->GetInjectPaintPass()->SetGetCurrentPigment([this]() -> PigmentType {
            return colorMixerEntity->GetMixerManager()->GetCurrentPigment();
        });

        colorMixerEntity->InitializeClearButton([this]() -> void {
            paintSimSubPipeline->MarkClear();
        });
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
        std::vector<Rendering::IRenderPass*> passes;
        passes.push_back(drawUIPass);
        passes.append_range(paintSimSubPipeline->GetRenderPasses());

        return passes;
    }
} // namespace Beer::System
