#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySeed.hpp"
#include <memory>

namespace Beer::System
{
    class MenuBarManager : public IEntityManager
    {
    public:
        BeerEvent<void()> OnNewSeed;

    private:
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;

        std::unique_ptr<Button> newSeedButton = nullptr;
        Function<void> clearHistory = nullptr;

        std::unique_ptr<Button> flyButton = nullptr;
        Function<void> saveMap = nullptr;

        std::unique_ptr<Button> backButton = nullptr;
        Function<void> onBackToTitle = nullptr;

        Function<void> enableBlock = nullptr;

        Rendering::FullscreenTransitionPass* transitionPass = nullptr;

        bool canFade = true;

    public:
        MenuBarManager(GalaxyMapBuffer* galaxyMapBuffer,
            Function<void> clearHistory,
            Function<void> saveMap,
            Function<void> onBackToTitle,
            Function<void> enableBlock)
            : galaxyMapBuffer(galaxyMapBuffer)
            , clearHistory(clearHistory)
            , saveMap(saveMap)
            , onBackToTitle(onBackToTitle)
            , enableBlock(enableBlock)
        {
            transitionPass = Rendering::IRenderPass::FetchFromRegister<Rendering::FullscreenTransitionPass>(Rendering::TRANSITION_PASS);
        }

        void InitializeButtons(UITransform* newSeedTransform,
            Rendering::Material* newSeedMaterial,
            UITransform* flyTransform,
            Rendering::Material* flyMaterial,
            UITransform* backTransform,
            Rendering::Material* backMaterial)
        {
            InitializeNewSeed(newSeedTransform, newSeedMaterial);
            InitializeFly(flyTransform, flyMaterial);
            InitializeBack(backTransform, backMaterial);
        }

        void Update() override
        {
        }

    private:
        void InitializeNewSeed(UITransform* newSeedTransform,
            Rendering::Material* newSeedMaterial);

        void InitializeFly(UITransform* flyTransform,
            Rendering::Material* flyMaterial);

        void InitializeBack(UITransform* backTransform,
            Rendering::Material* backMaterial);

        void InvokeFly();
    };
} // namespace Beer::System
