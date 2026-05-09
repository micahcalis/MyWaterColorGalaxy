#pragma once

#include "GalaxyComponent.hpp"
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

    private:
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;
        UITransform* mapTransform = nullptr;
        UITransform* sunTransform = nullptr;
        std::unique_ptr<GalaxySpriteFactory> factory = nullptr;
        Function<glm::vec4, ColorBarLevel> getColor = nullptr;

    public:
        GalaxyMapCursor(GalaxyMapBuffer* galaxyMapBuffer,
            UITransform* mapTransform,
            UITransform* sunTransform,
            Function<glm::vec4, ColorBarLevel> getColor);

        void Update(MouseInput input);
        uint32_t Place(const GalaxyComponentData& data, bool fromHistory = false);
        void Erase(uint32_t index, bool fromHistory = false);
        void SetSize(float normalizedVal);

        GalaxySpriteFactory* GetFactory() const { return factory.get(); };

    private:
        PixelRect GetCursorRect(glm::vec2 mousePos) const;
        glm::vec2 ToMapSpace(glm::vec2 pixelPoint) const;
        glm::vec2 ToPixelSpace(glm::vec2 mapPoint) const;
        glm::vec2 ScaleToPixelSize(glm::vec2 mapVec) const;
        GalaxyComponentData GetNewData(glm::vec2 mousePos) const;
    };
} // namespace Beer::System
