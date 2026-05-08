#pragma once

#include "GalaxyComponent.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyComponent.hpp"
#include "Rendering/Material/Material.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySeed.hpp"
#include <cstdint>
#include <unordered_map>

namespace Beer::System
{
    struct GalaxyComponentHitInfo
    {
    public:
        bool Hit = false;
        UITransform* Transform = nullptr;
        uint32_t Index = 0;
    };

    class GalaxyMapBuffer
    {
    private:
        GalaxySeed seed;
        GalaxySettings settings{};
        std::shared_ptr<Rendering::Material> mapMaterial = nullptr;
        std::shared_ptr<Rendering::Material> starMaterial = nullptr;
        Function<void, glm::vec2> updateStarPosition = nullptr;

        std::unordered_map<uint32_t, GalaxyComponent> componentMap;
        uint32_t nextComponentId = 0;

    public:
        GalaxyMapBuffer(GalaxySeed seed)
        {
            this->seed = seed;
            settings.UpdateSettings(seed);
            UpdateMaterials();
            ClearComponents();
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

        [[nodiscard]] bool HasComponent(uint32_t id) const
        {
            auto it = componentMap.find(id);
            return it != componentMap.end();
        }

        std::array<glm::vec4, 4> GetGalaxyColors()
        {
            return {settings.StarColor,
                settings.ColorA,
                settings.ColorB,
                settings.ColorC};
        }

        uint32_t AddComponent(GalaxyComponent component);
        void RemoveComponent(uint32_t id);
        GalaxyComponentData GetComponentData(uint32_t id) const;
        GalaxyComponentHitInfo CollisionCheck(const PixelRect& rect);
        std::vector<UIRenderItem> GetRenderItems() const;
        void SetColorByLevel(glm::vec4 newColor, ColorBarLevel level);

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

        void ClearComponents()
        {
            componentMap.clear();
        }
    };
} // namespace Beer::System
