#include "System/PaintTool/ToolBar/MapHistoryController.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Audio/SoundGlobalSettings.hpp"
#include "System/Components/UI/Button.hpp"

namespace Beer::System
{
    static const int MAX_HISTORY_LENGTH = 20;

    MapHistoryController::MapHistoryController(UITransform* undoTransform,
        Rendering::Material* undoMaterial,
        UITransform* redoTransform,
        Rendering::Material* redoMaterial,
        Function<uint32_t, const GalaxyComponentData&> addComponent,
        Function<void, uint32_t> eraseComponent)
        : addComponent(addComponent), eraseComponent(eraseComponent)
    {
        undoButton = std::make_unique<Button>(undoTransform, undoMaterial);
        redoButton = std::make_unique<Button>(redoTransform, redoMaterial);

        undoButton->SetOnClick([this]() -> void { Undo(); });
        redoButton->SetOnClick([this]() -> void { Redo(); });

        mapHistory.reserve(MAX_HISTORY_LENGTH);
        redoHistory.reserve(MAX_HISTORY_LENGTH);

        AudioSettings audioSettings{};
        audioSettings.Volume = SELECT_CLIP_VOLUME;
        selectClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_SelectButton",
            audioSettings);
    }

    void MapHistoryController::AddPlaceAction(uint32_t index, GalaxyComponentData data)
    {
        if (mapHistory.size() >= MAX_HISTORY_LENGTH)
        {
            mapHistory.erase(mapHistory.begin());
        }

        mapHistory.emplace_back(index, data, MapActionType::Place);
        redoHistory.clear();
    }

    void MapHistoryController::AddEraseAction(uint32_t index, GalaxyComponentData data)
    {
        if (mapHistory.size() >= MAX_HISTORY_LENGTH)
        {
            mapHistory.erase(mapHistory.begin());
        }

        mapHistory.emplace_back(index, data, MapActionType::Erase);
        redoHistory.clear();
    }

    void MapHistoryController::Undo()
    {
        if (mapHistory.size() == 0)
            return;

        MapAction action = mapHistory.back();
        mapHistory.pop_back();
        ExecuteMapAction(action, true);
        redoHistory.push_back(action);
        selectClip->Play();
    }

    void MapHistoryController::Redo()
    {
        if (redoHistory.size() == 0)
            return;

        MapAction action = redoHistory.back();
        redoHistory.pop_back();
        ExecuteMapAction(action, false);
        mapHistory.push_back(action);
        selectClip->Play();
    }

    void MapHistoryController::ExecuteMapAction(MapAction& action, bool isUndo)
    {
        if (isUndo)
        {
            if (action.Action == MapActionType::Place)
            {
                eraseComponent(action.ComponentIndex);
            } else if (action.Action == MapActionType::Erase)
            {
                action.ComponentIndex = addComponent(action.Data);
            }
        } else
        {
            if (action.Action == MapActionType::Place)
            {
                action.ComponentIndex = addComponent(action.Data);
            } else if (action.Action == MapActionType::Erase)
            {
                eraseComponent(action.ComponentIndex);
            }
        }
    }

    void MapHistoryController::ClearHistory()
    {
        mapHistory.clear();
        redoHistory.clear();
    }
} // namespace Beer::System
