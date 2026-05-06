#pragma once

#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
namespace Beer::System
{
    class GalaxyMapManager : public IEntityManager
    {
    private:
        GalaxyMapBuffer* galaxyBuffer = nullptr;
        UITransform* mapTransform = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        std::unique_ptr<GalaxyMapCursor> cursor = nullptr;

    public:
        GalaxyMapManager(GalaxyMapBuffer* galaxyMapBuffer,
            UITransform* mapTransform,
            Function<MouseInput> getMouseInput);

        void InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor);
        void Update() override;

        GalaxyMapCursor* GetCursor() const { return cursor.get(); }
        void SetBrushType(GalaxyBrushType type);
    };
} // namespace Beer::System
