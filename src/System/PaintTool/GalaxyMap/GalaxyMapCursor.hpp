#pragma once

#include "GalaxyComponent.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySpriteFactory.hpp"
#include <cstdarg>

namespace Beer::System
{
    class GalaxyMapCursor
    {
    public:
        BeerEvent<void(uint32_t, const GalaxyComponentData&, bool fromHistory)> OnComponentPlaced;
        BeerEvent<void(uint32_t, const GalaxyComponentData&, bool fromHistory)> OnComponentErased;

        GalaxyBrushType Brush = GalaxyBrushType::Planet;
        float Size = 0;
        bool CanUseCursor = false;
        std::shared_ptr<AudioClip> placeAudioClip = nullptr;
        std::shared_ptr<AudioClip> eraseAudioClip = nullptr;

    private:
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;
        UITransform* mapTransform = nullptr;
        UITransform* sunTransform = nullptr;
        std::unique_ptr<GalaxySpriteFactory> factory = nullptr;
        Function<glm::vec4, ColorBarLevel> getColor = nullptr;

        Function<void, glm::vec2, float> setZoom = nullptr;
        glm::vec2 zoomAnchor{};
        bool zooming = false;

        Function<void, glm::vec2> setPanning = nullptr;
        bool isPanning = false;
        glm::vec2 lastMousePos{};

    public:
        GalaxyMapCursor(GalaxyMapBuffer* galaxyMapBuffer,
            UITransform* mapTransform,
            UITransform* sunTransform,
            Function<glm::vec4, ColorBarLevel> getColor,
            Function<void, glm::vec2, float> setZoom,
            Function<void, glm::vec2> setPanning);

        void Update(MouseInput input,
            float zoomScale,
            glm::vec2 panning,
            bool insideRect);

        uint32_t Place(const GalaxyComponentData& data, bool fromHistory = false);
        void Erase(uint32_t index, bool fromHistory = false);
        void SetSize(float normalizedVal);
        float GetNormalizedSize() const;

        GalaxySpriteFactory* GetFactory() const { return factory.get(); };

    private:
        glm::vec2 ApplyZoom(glm::vec2 rawMouse,
            float zoomScale,
            glm::vec2 panning) const;

        PixelRect GetCursorRect(glm::vec2 mousePos) const;
        glm::vec2 ToMapSpace(glm::vec2 pixelPoint) const;
        glm::vec2 ToPixelSpace(glm::vec2 mapPoint) const;
        glm::vec2 ScaleToPixelSize(glm::vec2 mapVec) const;
        GalaxyComponentData GetNewData(glm::vec2 mousePos) const;
        void SetZoomAnchor(glm::vec2 zoomAnchor);
        void Zoom(glm::vec2 pixelPos);
        void Pan(glm::vec2 pixelPos);
    };
} // namespace Beer::System
