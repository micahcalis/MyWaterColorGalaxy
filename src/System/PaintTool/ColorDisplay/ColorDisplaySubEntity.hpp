#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    class ColorDisplaySubEntity
    {
    private:
        std::unique_ptr<UISubEntity> container = nullptr;

        std::vector<std::unique_ptr<UISubEntity>> colorEntities;
        std::vector<std::shared_ptr<Rendering::Material>> colorMaterials;
        std::shared_ptr<Rendering::Texture2D> colorTexture = nullptr;

    public:
        ColorDisplaySubEntity(UITransform containerTransform,
            UITransform* rootTransform);

        std::vector<std::shared_ptr<Rendering::Material>>& GetColorMaterials()
        {
            return colorMaterials;
        }

        std::vector<UIRenderItem> GetRenderItems() const
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.reserve(5);

            for (int i = 0; i < colorEntities.size(); i++)
            {
                renderItems.push_back(UIRenderItem(colorEntities[i]->GetTransform(), colorMaterials[i].get()));
            }

            return renderItems;
        }
    };
} // namespace Beer::System
