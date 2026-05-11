#pragma once

#include "Rendering/Material/Material.hpp"
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

    public:
        MenuBarManager(GalaxyMapBuffer* galaxyMapBuffer,
            Function<void> clearHistory,
            Function<void> saveMap)
            : galaxyMapBuffer(galaxyMapBuffer), clearHistory(clearHistory), saveMap(saveMap)
        {
        }

        void InitializeButtons(UITransform* newSeedTransform,
            Rendering::Material* newSeedMaterial,
            UITransform* flyTransform,
            Rendering::Material* flyMaterial)
        {
            InitializeNewSeed(newSeedTransform, newSeedMaterial);
            InitializeFly(flyTransform, flyMaterial);
        }

        void Update() override
        {
        }

    private:
        void InitializeNewSeed(UITransform* newSeedTransform,
            Rendering::Material* newSeedMaterial);

        void InitializeFly(UITransform* flyTransform,
            Rendering::Material* flyMaterial);

        void InvokeFly();
    };
} // namespace Beer::System
