#pragma once

#include "GalaxyBrushController.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace Beer::System
{
    class ToolBarManager : public IEntityManager
    {
    private:
        std::unordered_map<GalaxyBrushType, std::unique_ptr<GalaxyBrushController>> brushControllers;
        Function<void, GalaxyBrushType> setBrushType = nullptr;

        GalaxyBrushController* currentBrushController = nullptr;

    public:
        ToolBarManager(Function<void, GalaxyBrushType> setBrushType)
            : setBrushType(setBrushType)
        {
        }

        void CreateGalaxyBrushController(UITransform* transform,
            Rendering::Material* material,
            GalaxyBrushType brushType);

        void SetCurrentBrush(GalaxyBrushType type)
        {
            HandleNewBrushType(type);
        }

        void Update() override
        {
        }

    private:
        void HandleNewBrushType(GalaxyBrushType newType);
    };
} // namespace Beer::System
