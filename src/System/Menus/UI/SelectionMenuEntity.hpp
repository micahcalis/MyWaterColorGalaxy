#pragma once

#include "Rendering/Texture/Texture2D.hpp"
#include "SelectionMenuManager.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    class SelectionMenuEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Texture2D> headerTexture = nullptr;
        std::shared_ptr<Rendering::Material> headerMaterial = nullptr;
        std::unique_ptr<UISubEntity> headerEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> backgroundTexture = nullptr;
        std::shared_ptr<Rendering::Material> backgroundMaterial = nullptr;

        std::shared_ptr<Rendering::Texture2D> slotTexture = nullptr;
        std::vector<std::unique_ptr<UISubEntity>> slotEntities;
        std::vector<std::shared_ptr<Rendering::Material>> slotMaterials;

    public:
        SelectionMenuEntity();
        void InitializeSlots();

        SelectionMenuManager* GetSelectionMenuManager() const { return static_cast<SelectionMenuManager*>(manager.get()); };

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<SelectionMenuManager>();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMaterial.get()));
            renderItems.push_back(UIRenderItem(headerEntity->GetTransform(), headerMaterial.get()));

            for (int i = 0; i < slotEntities.size(); i++)
            {
                renderItems.push_back(UIRenderItem(slotEntities[i]->GetTransform(), slotMaterials[i].get()));
            }

            return renderItems;
        }
    };
} // namespace Beer::System
