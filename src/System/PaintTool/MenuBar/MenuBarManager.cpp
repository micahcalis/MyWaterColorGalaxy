#include "System/PaintTool/MenuBar/MenuBarManager.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "System/Base/Clock/Clock.hpp"
#include <memory>
#include <print>

namespace Beer::System
{
    static const float FADE_DURATION = 1.0f;

    void MenuBarManager::InitializeNewSeed(UITransform* newSeedTransform,
        Rendering::Material* newSeedMaterial)
    {
        newSeedButton = std::make_unique<Button>(newSeedTransform, newSeedMaterial);

        newSeedButton->SetOnClick([this]() -> void { OnNewSeed.Invoke(); });
        OnNewSeed.Subscribe([this]() -> void { galaxyMapBuffer->SetNewSeed(GalaxySeed()); });
        OnNewSeed.Subscribe(clearHistory);
    }

    void MenuBarManager::InitializeFly(UITransform* flyTransform,
        Rendering::Material* flyMaterial)
    {
        flyButton = std::make_unique<Button>(flyTransform, flyMaterial);
        flyButton->SetOnClick([this]() -> void { InvokeFly(); });
    }

    void MenuBarManager::InitializeBack(UITransform* backTransform,
        Rendering::Material* backMaterial)
    {
        backButton = std::make_unique<Button>(backTransform, backMaterial);
        backButton->SetOnClick(onBackToTitle);
    }

    void MenuBarManager::InvokeFly()
    {
        if (!canFade)
            return;

        if (!enableBlock)
            return;

        transitionPass->SetFade(Rendering::FadeState::In, 1.0f / FADE_DURATION);
        enableBlock();

        auto timer = Clock::Timer(FADE_DURATION);
        timer->OnTimerComplete.Subscribe(saveMap);
        timer->Start();

        canFade = false;
    }
} // namespace Beer::System
