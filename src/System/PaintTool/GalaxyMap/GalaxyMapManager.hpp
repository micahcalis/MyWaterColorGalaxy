#pragma once

#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
namespace Beer::System
{
    struct CursorState
    {
    public:
        bool IsActive = false;
        bool CanUseCursor = false;
        GalaxyBrushType Brush = GalaxyBrushType::Planet;
        float BrushCanvasSize = 0.05f;
    };

    class GalaxyMapManager : public IEntityManager
    {
    private:
        GalaxyMapBuffer* galaxyBuffer = nullptr;
        UITransform* mapTransform = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        std::unique_ptr<GalaxyMapCursor> cursor = nullptr;
        bool isActive = false;

    public:
        GalaxyMapManager(GalaxyMapBuffer* galaxyMapBuffer,
            UITransform* mapTransform,
            Function<MouseInput> getMouseInput);

        void InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor);
        void Update() override;

        GalaxyMapCursor* GetCursor() const { return cursor.get(); }
        void SetBrushType(GalaxyBrushType type);

        CursorState GetCursorState() const
        {
            return {isActive,
                cursor->CanUseCursor,
                cursor->Brush,
                cursor->Size * mapTransform->Scale.x};
        };
    };
} // namespace Beer::System
