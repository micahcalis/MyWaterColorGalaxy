#pragma once

#include "GalaxyBrushController.hpp"
#include "MapHistoryController.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include <memory>
#include <unordered_map>
#include "System/Audio/SoundGlobalSettings.hpp"

namespace Beer::System
{
    class ToolBarManager : public IEntityManager
    {
    private:
        std::unordered_map<GalaxyBrushType, std::unique_ptr<GalaxyBrushController>> brushControllers;
        Function<void, GalaxyBrushType> setBrushType = nullptr;

        GalaxyBrushController* currentBrushController = nullptr;

        std::unique_ptr<MapHistoryController> mapHistoryController = nullptr;

        std::shared_ptr<AudioClip> selectClip = nullptr;

    public:
        ToolBarManager(Function<void, GalaxyBrushType> setBrushType)
            : setBrushType(setBrushType)
        {
            AudioSettings audioSettings{};
            audioSettings.Volume = SELECT_CLIP_VOLUME;
            selectClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_SelectButton",
                audioSettings);
        }

        void CreateGalaxyBrushController(UITransform* transform,
            Rendering::Material* material,
            GalaxyBrushType brushType);

        void SetCurrentBrush(GalaxyBrushType type)
        {
            HandleNewBrushType(type);
        }

        void CreateHistoryButtons(UITransform* undoTransform,
            Rendering::Material* undoMaterial,
            UITransform* redoTransform,
            Rendering::Material* redoMaterial,
            Function<uint32_t, const GalaxyComponentData&> addComponent,
            Function<void, uint32_t> eraseComponent);

        void Update() override
        {
        }

        [[nodiscard]] MapHistoryController* GetHistoryController() const
        {
            if (mapHistoryController == nullptr)
            {
                return nullptr;
            }

            return mapHistoryController.get();
        }

    private:
        void HandleNewBrushType(GalaxyBrushType newType);
    };
} // namespace Beer::System
