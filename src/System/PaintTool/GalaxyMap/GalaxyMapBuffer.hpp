#pragma once

#include "GalaxyComponent.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyComponent.hpp"
#include "Rendering/Material/Material.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySeed.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <cstdint>
#include <print>
#include <unordered_map>
#include <utility>

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
        GalaxySeed seed{};
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

        void ApplySerializableGalaxy(const SerializableGalaxy& serializableGalaxy);

        void SetNewSeed(GalaxySeed seed)
        {
            this->seed = seed;
            settings.UpdateSettings(seed);
            UpdateMaterials();
            ClearComponents();
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

        std::pair<glm::vec2, float> GetSunStarTransformData() const
        {
            return {settings.StarPosition, settings.StarSize};
        }

        SerializableGalaxy GetSerializableGalaxy() const;
        uint32_t AddComponent(GalaxyComponent component);
        void RemoveComponent(uint32_t id);
        GalaxyComponentData GetComponentData(uint32_t id) const;
        GalaxyComponentHitInfo CollisionCheck(const PixelRect& rect);
        std::vector<UIRenderItem> GetRenderItems() const;
        void SetColorByLevel(glm::vec4 newColor, ColorBarLevel level);

        void PrintComponents()
        {
            for (auto& comp : componentMap)
            {
                std::println("id: {}", comp.first);
                std::println("UI Pixel Rect TL: {}, {}", comp.second.Entity->GetTransform()->Rect.TopLeft.x, comp.second.Entity->GetTransform()->Rect.TopLeft.y);
                std::println("UI Pixel Rect TR: {}, {}", comp.second.Entity->GetTransform()->Rect.TopRight.x, comp.second.Entity->GetTransform()->Rect.TopRight.y);
                std::println("UI Pixel Rect BL: {}, {}", comp.second.Entity->GetTransform()->Rect.BotLeft.x, comp.second.Entity->GetTransform()->Rect.BotLeft.y);
                std::println("UI Pixel Rect BR: {}, {}", comp.second.Entity->GetTransform()->Rect.BotRight.x, comp.second.Entity->GetTransform()->Rect.BotRight.y);
            }
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
                mapMaterial->SetVector("_RingShear", glm::vec4(settings.OrbitShear, 0, 0));
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
