#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
namespace Beer::System
{
    class LaunchButtonEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> backgroundMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> backgroundTexture = nullptr;

        std::unique_ptr<UISubEntity> flyButtonEntity = nullptr;
        std::shared_ptr<Rendering::Material> flyButtonMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> flyButtonTexture = nullptr;

    public:
        LaunchButtonEntity();

        void InitializeManager() override
        {
        }

        UITransform* GetButtonTransform() const { return flyButtonEntity->GetTransform(); }
        Rendering::Material* GetButtonMaterial() const { return flyButtonMaterial.get(); }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMaterial.get()));
            renderItems.push_back(UIRenderItem(flyButtonEntity->GetTransform(), flyButtonMaterial.get()));
            return renderItems;
        }
    };
} // namespace Beer::System
