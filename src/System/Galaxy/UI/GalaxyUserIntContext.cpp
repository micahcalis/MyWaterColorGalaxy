#include "System/Galaxy/UI/GalaxyUserIntContext.hpp"
#include "ControlsDisplayEntity.hpp"
#include "OptionsDisplayEntity.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include <print>

namespace Beer::System
{
    void GalaxyUserIntContext::Load()
    {
        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        InitializeControlsDisplay();
        InitializeOptionsDisplay();
    }

    void GalaxyUserIntContext::Update()
    {
        if (controlsDisplayEntity != nullptr)
        {
            controlsDisplayEntity->Update();
        }

        if (optionsDisplayEntity != nullptr)
        {
            optionsDisplayEntity->Update();
        }
    }

    void GalaxyUserIntContext::SetDisplaysEnabled(bool enabled)
    {
        if (controlsDisplayEntity != nullptr)
        {
            controlsDisplayEntity->SetTreeEnabled(enabled);
        }

        if (optionsDisplayEntity != nullptr)
        {
            optionsDisplayEntity->SetTreeEnabled(enabled);
        }
    }

    std::vector<Rendering::IRenderPass*> GalaxyUserIntContext::GetRenderPasses()
    {
        return {drawUIPass};
    }

    void GalaxyUserIntContext::InitializeControlsDisplay()
    {
        controlsDisplayEntity = registry.CreateEntity<ControlsDisplayEntity>();
    }

    void GalaxyUserIntContext::InitializeOptionsDisplay()
    {
        optionsDisplayEntity = registry.CreateEntity<OptionsDisplayEntity>();
    }
} // namespace Beer::System
