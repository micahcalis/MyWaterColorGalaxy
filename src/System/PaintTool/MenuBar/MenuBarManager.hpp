#pragma once

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

    public:
        MenuBarManager(GalaxyMapBuffer* galaxyMapBuffer,
            Function<void> clearHistory)
            : galaxyMapBuffer(galaxyMapBuffer), clearHistory(clearHistory)
        {
        }

        void InitializeButtons(UITransform* newSeedTransform,
            Rendering::Material* newSeedMaterial)
        {
            if (newSeedButton != nullptr)
            {
                return;
            }

            newSeedButton = std::make_unique<Button>(newSeedTransform, newSeedMaterial);

            newSeedButton->SetOnClick([this]() -> void { OnNewSeed.Invoke(); });
            OnNewSeed.Subscribe([this]() -> void { galaxyMapBuffer->SetNewSeed(GalaxySeed()); });
            OnNewSeed.Subscribe(clearHistory);
        }

        void Update() override
        {
        }
    };
} // namespace Beer::System
