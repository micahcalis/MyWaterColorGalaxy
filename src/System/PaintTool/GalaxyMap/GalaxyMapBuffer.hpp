#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySeed.hpp"
#include <cstdint>

namespace Beer::System
{
    class GalaxyMapBuffer
    {
    private:
        GalaxySeed seed;
        GalaxySettings settings{};
        std::shared_ptr<Rendering::Material> mapMaterial = nullptr;
        std::shared_ptr<Rendering::Material> starMaterial = nullptr;
        Function<void, glm::vec2> updateStarPosition = nullptr;

    public:
        GalaxyMapBuffer(GalaxySeed seed)
        {
            this->seed = seed;
            settings.UpdateSettings(seed);
            UpdateMaterials();
        }

        void SetNewSeed(GalaxySeed seed)
        {
            this->seed = seed;
            settings.UpdateSettings(seed);
            UpdateMaterials();
        }

        void SetMapMaterial(std::shared_ptr<Rendering::Material> mapMaterial)
        {
            this->mapMaterial = mapMaterial;
            UpdateMaterials();
        }

        void SetStarMaterial(std::shared_ptr<Rendering::Material> starMaterial,
            Function<void, glm::vec2> updateStarPosition)
        {
            this->starMaterial = starMaterial;
            this->updateStarPosition = updateStarPosition;
            UpdateMaterials();
        }

    private:
        void UpdateMaterials()
        {
            if (mapMaterial != nullptr)
            {
                mapMaterial->SetInt("_GalaxySeed", static_cast<int32_t>(seed.ColorSeed));
                mapMaterial->SetColor("_ColorA", settings.ColorA);
                mapMaterial->SetColor("_ColorB", settings.ColorB);
                mapMaterial->SetColor("_ColorC", settings.ColorC);
                mapMaterial->SetVector("_GalaxyCenter", glm::vec4(settings.StarPosition, 0, 0));
            }

            if (starMaterial != nullptr && updateStarPosition != nullptr)
            {
                starMaterial->SetInt("_StarSeed", static_cast<int32_t>(seed.StarSeed));
                starMaterial->SetColor("_StarColor", settings.StarColor);
                starMaterial->SetFloat("_StarSize", settings.StarSize);
                updateStarPosition(settings.StarPosition);
            }
        }
    };
} // namespace Beer::System
