#pragma once

#include "ControlsDisplayEntity.hpp"
#include "ExitPhotoModeEntity.hpp"
#include "OptionsDisplayEntity.hpp"
#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "System/Context/IContext.hpp"
#include "System/Delegates/BeerEvent.hpp"

namespace Beer::System
{
    class GalaxyUserIntContext : public IContext
    {
    public:
        BeerEvent<void()> OnReturnClicked;
        BeerEvent<void()> OnPhotoModeToggled;

    private:
        Rendering::DrawUIPass* drawUIPass = nullptr;
        ControlsDisplayEntity* controlsDisplayEntity = nullptr;
        OptionsDisplayEntity* optionsDisplayEntity = nullptr;
        ExitPhotoModeEntity* exitPhotoModeEntity = nullptr;

    public:
        GalaxyUserIntContext()
        {
        }

        void Load() override;
        void Update() override;
        void SetDisplaysEnabled(bool enabled);
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;

    private:
        void InitializeControlsDisplay();
        void InitializeOptionsDisplay();
        void InitializeExitPhotoMode();
    };
} // namespace Beer::System
