#include "System/PaintTool/MenuBar/MenuBarManager.hpp"
#include <memory>

namespace Beer::System
{
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
        saveMap();
    }
} // namespace Beer::System
