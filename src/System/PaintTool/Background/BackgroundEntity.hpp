#pragma once

#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
namespace Beer::System
{
    class BackgroundEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> backgroundMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> backgroundTexture = nullptr;

    public:
        BackgroundEntity();

    protected:
        void InitializeManager() override
        {
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMaterial.get()));
            return renderItems;
        }
    };
} // namespace Beer::System
