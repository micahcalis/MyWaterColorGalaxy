#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyComponent.hpp"
#include <vector>

namespace Beer::System
{
    enum class MapActionType
    {
        Place,
        Erase
    };

    struct MapAction
    {
    public:
        uint32_t ComponentIndex;
        GalaxyComponentData Data;
        MapActionType Action;

    public:
        MapAction(uint32_t componentIndex,
            GalaxyComponentData data,
            MapActionType action)
            : ComponentIndex(componentIndex), Data(data), Action(action)
        {
        }
    };

    class MapHistoryController
    {
    private:
        std::unique_ptr<Button> undoButton = nullptr;
        std::unique_ptr<Button> redoButton = nullptr;

        std::vector<MapAction> mapHistory;
        std::vector<MapAction> redoHistory;

        Function<uint32_t, const GalaxyComponentData&> addComponent = nullptr;
        Function<void, uint32_t> eraseComponent = nullptr;

        std::shared_ptr<AudioClip> selectClip = nullptr;

    public:
        MapHistoryController(UITransform* undoTransform,
            Rendering::Material* undoMaterial,
            UITransform* redoTransform,
            Rendering::Material* redoMaterial,
            Function<uint32_t, const GalaxyComponentData&> addComponent,
            Function<void, uint32_t> eraseComponent);

        void AddPlaceAction(uint32_t index, GalaxyComponentData data);
        void AddEraseAction(uint32_t index, GalaxyComponentData data);
        void ClearHistory();

    private:
        void Undo();
        void Redo();
        void ExecuteMapAction(MapAction& action, bool isUndo);
    };
} // namespace Beer::System
