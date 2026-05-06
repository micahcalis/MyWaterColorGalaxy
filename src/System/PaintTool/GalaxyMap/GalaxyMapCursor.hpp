#pragma once

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
        BeerEvent<void()> OnComponentPlaced;
        GalaxyBrushType Brush = GalaxyBrushType::Planet;
        float Size = 0;

    private:
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;
        UITransform* mapTransform = nullptr;
        bool canPlace;
        std::unique_ptr<GalaxySpriteFactory> factory = nullptr;

    public:
        GalaxyMapCursor(GalaxyMapBuffer* galaxyMapBuffer,
            UITransform* mapTransform,
            Function<glm::vec4, ColorBarLevel> getColor);

        void Update(MouseInput input);

    public:
        [[nodiscard]] bool GetCanPlace() const { return canPlace; }

    private:
        void Place(const glm::vec2 mousePos);
        PixelRect GetCursorRect(glm::vec2 mousePos) const;
        glm::vec2 ToMapSpace(glm::vec2 pixelPoint) const;
        glm::vec2 ToPixelSpace(glm::vec2 mapPoint) const;
        glm::vec2 ScaleToPixelSize(glm::vec2 mapVec) const;
    };
} // namespace Beer::System
