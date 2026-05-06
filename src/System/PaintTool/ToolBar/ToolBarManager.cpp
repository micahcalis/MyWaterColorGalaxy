#include "System/PaintTool/ToolBar/ToolBarManager.hpp"
#include "GalaxyBrushController.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"

namespace Beer::System
{
    void ToolBarManager::CreateGalaxyBrushController(UITransform* transform,
        Rendering::Material* material,
        GalaxyBrushType brushType)
    {
        if (brushControllers.contains(brushType))
        {
            throw std::runtime_error(std::format("Galaxy Brush Already Initialized: {}", magic_enum::enum_name(brushType)));
        }

        std::unique_ptr<GalaxyBrushController> controller = std::make_unique<GalaxyBrushController>(transform,
            material,
            brushType);

        controller->OnBrushClicked.Subscribe(
            [this](GalaxyBrushType type) { HandleNewBrushType(type); });

        brushControllers[brushType] = std::move(controller);
    }

    void ToolBarManager::HandleNewBrushType(GalaxyBrushType newType)
    {
        setBrushType(newType);

        if (currentBrushController != nullptr)
        {
            currentBrushController->SetActive(false);
        }

        currentBrushController = brushControllers.at(newType).get();
        currentBrushController->SetActive(true);
    }
} // namespace Beer::System
