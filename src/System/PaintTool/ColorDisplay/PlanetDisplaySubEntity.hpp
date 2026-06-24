#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace Beer::System
{
    class PlanetDisplaySubEntity
    {
    private:
        std::unique_ptr<UISubEntity> container = nullptr;
        std::shared_ptr<Rendering::Material> containerMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> containerTexture = nullptr;

        std::unique_ptr<UISubEntity> displayEntity = nullptr;
        std::shared_ptr<Rendering::Material> displayMaterial = nullptr;
        std::unordered_map<GalaxyBrushType, std::shared_ptr<Rendering::Texture2D>> spriteMap;
        std::unordered_map<GalaxyBrushType, std::shared_ptr<Rendering::Texture2D>> maskMap;

    public:
        PlanetDisplaySubEntity(UITransform* parent);

        std::shared_ptr<Rendering::Material> GetDisplayMaterial() const { return displayMaterial; }

        std::vector<UIRenderItem> GetRenderItems() const
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.reserve(2);

            renderItems.push_back(UIRenderItem(container->GetTransform(), containerMaterial.get()));
            renderItems.push_back(UIRenderItem(displayEntity->GetTransform(), displayMaterial.get()));

            return renderItems;
        }

        Rendering::Texture2D* GetSprite(GalaxyBrushType type, bool isMask) const
        {
            auto map = isMask ? maskMap : spriteMap;
            auto it = map.find(type);

            if (it == map.end())
            {
                throw std::runtime_error("Trying to get unassigned Sprite from PlanetDisplaySubEntity map!");
            }

            return it->second.get();
        }

    private:
        void InitializeSpriteMaps();
    };
} // namespace Beer::System
