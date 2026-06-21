#include "System/PaintTool/MenuBar/MenuBarManager.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "System/Audio/SoundGlobalSettings.hpp"
#include "System/Base/Clock/Clock.hpp"
#include <memory>
#include <print>

namespace Beer::System
{
    static const float FADE_OUT_DURATION = 2.0f;
    static const float FADE_IN_DURATION = 0.5f;

    MenuBarManager::MenuBarManager(GalaxyMapBuffer* galaxyMapBuffer,
        Function<void> clearHistory,
        Function<void> saveMap,
        Function<void> onBackToTitle,
        Function<void> enableBlock,
        Rendering::FadeState initialFadeState)
        : galaxyMapBuffer(galaxyMapBuffer)
        , clearHistory(clearHistory)
        , saveMap(saveMap)
        , onBackToTitle(onBackToTitle)
        , enableBlock(enableBlock)
    {
        transitionPass = Rendering::IRenderPass::FetchFromRegister<Rendering::FullscreenTransitionPass>(Rendering::TRANSITION_PASS);

        if (initialFadeState == Rendering::FadeState::In)
        {
            transitionPass->SetFade(Rendering::FadeState::Out, 1.0f / FADE_IN_DURATION);

            auto timer = Clock::Timer(FADE_IN_DURATION);
            timer->OnTimerComplete.Subscribe([this]() -> void { isFadingIn = false; });
            timer->Start();
        } else
        {
            isFadingIn = false;
        }

        AudioSettings audioSettings{};
        audioSettings.Volume = SELECT_CLIP_VOLUME;
        selectClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_SelectButton",
            audioSettings);

        audioSettings.Volume = TRANSITION_VOLUME;
        transitionClip = std::make_shared<AudioClip>("SoundEffects/Game/Audio_HyperSpace",
            audioSettings);
    }

    void MenuBarManager::InitializeNewSeed(UITransform* newSeedTransform,
        Rendering::Material* newSeedMaterial)
    {
        newSeedButton = std::make_unique<Button>(newSeedTransform, newSeedMaterial);

        newSeedButton->SetOnClick([this]() -> void {
            OnNewSeed.Invoke();
            selectClip->Play();
        });

        OnNewSeed.Subscribe([this]() -> void { galaxyMapBuffer->SetNewSeed(GalaxySeed()); });
        OnNewSeed.Subscribe(clearHistory);
    }

    void MenuBarManager::InitializeFly(UITransform* flyTransform,
        Rendering::Material* flyMaterial)
    {
        flyButton = std::make_unique<Button>(flyTransform, flyMaterial);
        flyButton->SetOnClick([this]() -> void {
            InvokeFly();
            selectClip->Play();
            transitionClip->Play();
        });
    }

    void MenuBarManager::InitializeBack(UITransform* backTransform,
        Rendering::Material* backMaterial)
    {
        backButton = std::make_unique<Button>(backTransform, backMaterial);
        backButton->SetOnClick([this]() -> void {
            onBackToTitle();
            selectClip->Play();
        });
    }

    void MenuBarManager::InvokeFly()
    {
        if (!canFade)
            return;

        if (!enableBlock)
            return;

        if (isFadingIn)
            return;

        transitionPass->SetFade(Rendering::FadeState::In, 1.0f / FADE_OUT_DURATION);
        auto colors = galaxyMapBuffer->GetGalaxyColors();
        transitionPass->GetMaterial()->SetColor("_ColorA", colors[0]);
        transitionPass->GetMaterial()->SetColor("_ColorB", colors[1]);
        transitionPass->GetMaterial()->SetColor("_ColorC", colors[2]);
        transitionPass->GetMaterial()->SetColor("_ColorD", colors[3]);
        enableBlock();

        auto timer = Clock::Timer(FADE_OUT_DURATION);
        timer->OnTimerComplete.Subscribe(saveMap);
        timer->Start();

        canFade = false;
    }
} // namespace Beer::System
