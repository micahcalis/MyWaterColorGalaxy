#include "System/Menus/UI/TitleUserIntContext.hpp"
#include "MainMenuEntity.hpp"
#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Delegates/Delegate.hpp"
#include <print>

namespace Beer::System
{
    void TitleUserIntContext::Load()
    {
        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        InitializeMainMenu();
    }

    void TitleUserIntContext::Update()
    {
        if (mainMenuEntity != nullptr)
        {
            mainMenuEntity->Update();
        }
    }

    std::vector<Rendering::IRenderPass*> TitleUserIntContext::GetRenderPasses()
    {
        return {drawUIPass};
    }

    void TitleUserIntContext::InitializeMainMenu()
    {
        mainMenuEntity = registry.CreateEntity<MainMenuEntity>();
        mainMenuEntity->InitializeButtons();

        Function<void> onStart = [this]() -> void { std::println("start"); };
        mainMenuEntity->GetMainMenuManager()->OnStart.Subscribe(onStart);

        Function<void> onExit = [this]() -> void { OnCloseApplication.Invoke(); };
        mainMenuEntity->GetMainMenuManager()->OnExit.Subscribe(onExit);
    }
} // namespace Beer::System
