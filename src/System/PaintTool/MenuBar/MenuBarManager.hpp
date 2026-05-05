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
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;
        BeerEvent<void()> OnNewSeed;

    private:
        std::unique_ptr<Button> newSeedButton = nullptr;

    public:
        MenuBarManager(GalaxyMapBuffer* galaxyMapBuffer)
            : galaxyMapBuffer(galaxyMapBuffer)
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
        }

        void Update() override
        {
        }
    };
} // namespace Beer::System
