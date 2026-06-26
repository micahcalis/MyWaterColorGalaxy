#pragma once

#include "GalaxyMapZoomer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"
#include "System/PaintTool/HelpToggle/HelpContainer.hpp"
#include "System/PaintTool/HelpToggle/HelpToggle.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"

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
        , public HelpContainer
    {
    public:
        BeerEvent<void(GalaxyBrushType)> OnNewBrush;

    private:
        GalaxyMapBuffer* galaxyBuffer = nullptr;
        UITransform* mapTransform = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        UITransform* playerIndicatorTransform = nullptr;
        std::unique_ptr<GalaxyMapCursor> cursor = nullptr;
        std::unique_ptr<GalaxyMapZoomer> zoomer = nullptr;
        bool isActive = false;

    public:
        GalaxyMapManager(GalaxyMapBuffer* galaxyMapBuffer,
            UITransform* mapTransform,
            Function<MouseInput> getMouseInput,
            UITransform* playerIndicatorTransform);

        void InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor,
            UITransform* sunTranform);

        void Update() override;

        GalaxyMapCursor* GetCursor() const { return cursor.get(); }
        GalaxyMapZoomer* GetZoomer() const { return zoomer.get(); }

        void SetBrushType(GalaxyBrushType type);
        void ReloadFromSerialized(const SerializableGalaxyMap& serializedData);

        CursorState GetCursorState() const
        {
            return {isActive,
                cursor->CanUseCursor,
                cursor->Brush,
                cursor->Size * mapTransform->Scale.x * zoomer->Zoom};
        };

    private:
        void SetPlayerTransformPosition(glm::vec3 serializedPosition);
    };
} // namespace Beer::System
