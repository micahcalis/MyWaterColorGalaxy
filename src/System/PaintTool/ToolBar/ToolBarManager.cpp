#include "System/PaintTool/ToolBar/ToolBarManager.hpp"
#include "GalaxyBrushController.hpp"
#include "MapHistoryController.hpp"
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

        Function<void, GalaxyBrushType> onBrushClicked = [this](GalaxyBrushType type) {
            HandleNewBrushType(type);
            selectClip->Play();
        };

        controller->OnBrushClicked.Subscribe(onBrushClicked);

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

    void ToolBarManager::CreateHistoryButtons(UITransform* undoTransform,
        Rendering::Material* undoMaterial,
        UITransform* redoTransform,
        Rendering::Material* redoMaterial,
        Function<uint32_t, const GalaxyComponentData&> addComponent,
        Function<void, uint32_t> eraseComponent)
    {
        mapHistoryController = std::make_unique<MapHistoryController>(undoTransform,
            undoMaterial,
            redoTransform,
            redoMaterial,
            addComponent,
            eraseComponent);
    }
} // namespace Beer::System
