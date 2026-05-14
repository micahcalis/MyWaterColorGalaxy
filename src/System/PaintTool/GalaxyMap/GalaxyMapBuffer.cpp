#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <stdexcept>

namespace Beer::System
{
    void GalaxyMapBuffer::ApplySerializableGalaxy(const SerializableGalaxy& serializableGalaxy)
    {
        ClearComponents();

        seed.StarSeed = serializableGalaxy.StarSeed;
        seed.ColorSeed = serializableGalaxy.ColorSeed;

        settings.StarColor = serializableGalaxy.StarColor;
        settings.StarPosition = serializableGalaxy.StarPosition;
        settings.StarSize = serializableGalaxy.StarSize;

        settings.ColorA = serializableGalaxy.ColorA;
        settings.ColorB = serializableGalaxy.ColorB;
        settings.ColorC = serializableGalaxy.ColorC;

        settings.OrbitShear = serializableGalaxy.OrbitShear;

        UpdateMaterials();
    }

    SerializableGalaxy GalaxyMapBuffer::GetSerializableGalaxy() const
    {
        SerializableGalaxy serializable{};
        serializable.StarSeed = seed.StarSeed;
        serializable.ColorSeed = seed.ColorSeed;

        serializable.StarColor = settings.StarColor;
        serializable.StarPosition = settings.StarPosition;
        serializable.StarSize = settings.StarSize;

        serializable.ColorA = settings.ColorA;
        serializable.ColorB = settings.ColorB;
        serializable.ColorC = settings.ColorC;

        serializable.OrbitShear = settings.OrbitShear;

        SerializableGalaxyComponent compSerializable{};

        for (const auto& component : componentMap)
        {
            compSerializable.Id = component.first;
            compSerializable.TypeIndex = static_cast<uint32_t>(component.second.Data.Brush);
            compSerializable.Colors = component.second.Data.Colors;
            compSerializable.Scale = component.second.Data.Scale;
            compSerializable.Position = component.second.Data.Position;
            compSerializable.Tilt = component.second.Data.Tilt;

            serializable.Components.push_back(compSerializable);
        }

        return serializable;
    }

    uint32_t GalaxyMapBuffer::AddComponent(GalaxyComponent component)
    {
        nextComponentId++;
        component.SetId(nextComponentId);
        componentMap.emplace(nextComponentId, std::move(component));
        return nextComponentId;
    }

    void GalaxyMapBuffer::RemoveComponent(uint32_t id)
    {
        auto it = componentMap.find(id);

        if (it != componentMap.end())
        {
            componentMap.erase(id);
            return;
        }

        throw std::runtime_error("Trying to remove non-existent Galaxy Component!");
    }

    GalaxyComponentData GalaxyMapBuffer::GetComponentData(uint32_t id) const
    {
        auto it = componentMap.find(id);

        if (it != componentMap.end())
        {
            return componentMap.at(id).Data;
        }

        throw std::runtime_error("Trying to get component data from non-existent Galaxy Component!");
    }

    GalaxyComponentHitInfo GalaxyMapBuffer::CollisionCheck(const PixelRect& rect)
    {
        GalaxyComponentHitInfo hitInfo{};

        for (const auto& component : componentMap)
        {
            const PixelRect& compare = component.second.Entity->GetTransform()->Rect;

            if (rect.Intersects(compare))
            {
                hitInfo.Hit = true;
                hitInfo.Transform = component.second.Entity->GetTransform();
                hitInfo.Index = component.first;
                break;
            }
        }

        return hitInfo;
    }

    std::vector<UIRenderItem> GalaxyMapBuffer::GetRenderItems() const
    {
        std::vector<UIRenderItem> renderItems;
        renderItems.reserve(componentMap.size());

        for (auto& component : componentMap)
        {
            renderItems.push_back(UIRenderItem(component.second.Entity->GetTransform(),
                component.second.Material.get()));
        }

        return renderItems;
    }

    void GalaxyMapBuffer::SetColorByLevel(glm::vec4 newColor, ColorBarLevel level)
    {
        switch (level)
        {
        case ColorBarLevel::Primary:
            settings.StarColor = newColor;
            break;
        case ColorBarLevel::Secondary:
            settings.ColorA = newColor;
            break;
        case ColorBarLevel::Tertiary:
            settings.ColorB = newColor;
            break;
        case ColorBarLevel::Quaternary:
            settings.ColorC = newColor;
            break;
        }

        UpdateMaterials();
    }
} // namespace Beer::System
