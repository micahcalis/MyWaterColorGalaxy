#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <stdexcept>

namespace Beer::System
{
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

} // namespace Beer::System
