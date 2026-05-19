#pragma once

#include "ControlsDisplayEntity.hpp"
#include "OptionsDisplayEntity.hpp"
#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "System/Context/IContext.hpp"

namespace Beer::System
{
    class GalaxyUserIntContext : public IContext
    {
    private:
        Rendering::DrawUIPass* drawUIPass = nullptr;
        ControlsDisplayEntity* controlsDisplayEntity = nullptr;
        OptionsDisplayEntity* optionsDisplayEntity = nullptr;

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
    };
} // namespace Beer::System
