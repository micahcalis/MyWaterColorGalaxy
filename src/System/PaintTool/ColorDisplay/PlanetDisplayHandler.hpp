#pragma once

#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/ColorDisplay/PlanetDisplaySubEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
namespace Beer::System
{
    static const std::array<std::string, 4> PLANET_COLOR_PROPERTIES = {
        "_PrimaryColor",
        "_SecondaryColor",
        "_TertiaryColor",
        "_QuaternaryColor"};

    class PlanetDisplayHandler
    {
    private:
        PlanetDisplaySubEntity* planetDisplaySubEntity = nullptr;
        std::shared_ptr<Rendering::Material> displayMaterial = nullptr;

    public:
        PlanetDisplayHandler(PlanetDisplaySubEntity* planetDisplaySubEntity)
            : planetDisplaySubEntity(planetDisplaySubEntity)
        {
            displayMaterial = planetDisplaySubEntity->GetDisplayMaterial();
        }

        void SetType(GalaxyBrushType type)
        {
            displayMaterial->SetTexture("_SpriteTex", planetDisplaySubEntity->GetSprite(type, false), false);
            displayMaterial->SetTexture("_ColorMask", planetDisplaySubEntity->GetSprite(type, true), false);
        }

        void SetColor(glm::vec4 color, ColorBarLevel level)
        {
            displayMaterial->SetColor(PLANET_COLOR_PROPERTIES[(uint32_t(level))], color);
        }
    };
} // namespace Beer::System
