#include "System/Galaxy/UI/GalaxyUserIntContext.hpp"
#include "ControlsDisplayEntity.hpp"
#include "OptionsDisplayEntity.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Base/Input/Input.hpp"
#include <print>

namespace Beer::System
{
    void GalaxyUserIntContext::Load()
    {
        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        InitializeControlsDisplay();
        InitializeOptionsDisplay();
        InitializeExitPhotoMode();
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

        if (exitPhotoModeEntity != nullptr)
        {
            exitPhotoModeEntity->Update();
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

        if (exitPhotoModeEntity != nullptr)
        {
            exitPhotoModeEntity->SetTreeEnabled(!enabled);
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

        if (Input::Mode() == InputMode::PenDisplay)
        {
            Function<void> invokeReturn = [this]() -> void {
                OnReturnClicked.Invoke();
            };

            Function<void> invokePhotoModeToggle = [this]() -> void {
                OnPhotoModeToggled.Invoke();
            };

            optionsDisplayEntity->InitializeButtons(invokeReturn, invokePhotoModeToggle);
        }
    }

    void GalaxyUserIntContext::InitializeExitPhotoMode()
    {
        exitPhotoModeEntity = registry.CreateEntity<ExitPhotoModeEntity>();

        if (Input::Mode() == InputMode::PenDisplay)
        {
            Function<void> invokePhotoModeToggle = [this]() -> void {
                OnPhotoModeToggled.Invoke();
            };

            exitPhotoModeEntity->InitializeButton(invokePhotoModeToggle);
        }

        exitPhotoModeEntity->SetTreeEnabled(false);
    }
} // namespace Beer::System
