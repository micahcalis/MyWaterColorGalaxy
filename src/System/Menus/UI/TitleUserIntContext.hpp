#pragma once

#include "System/Menus/UI/MainMenuEntity.hpp"
#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "System/Context/IContext.hpp"

namespace Beer::System
{
    class TitleUserIntContext : public IContext
    {
    public:
        BeerEvent<void()> OnOpenSelection;
        BeerEvent<void()> OnCloseApplication;

    private:
        Rendering::DrawUIPass* drawUIPass = nullptr;
        MainMenuEntity* mainMenuEntity = nullptr;

    public:
        TitleUserIntContext()
        {
        }

        void Load() override;
        void Update() override;
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;

    private:
        void InitializeMainMenu();
    };
} // namespace Beer::System
