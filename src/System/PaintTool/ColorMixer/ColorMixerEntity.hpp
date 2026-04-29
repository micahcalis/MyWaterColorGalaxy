#pragma once

#include "ColorMixerManager.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include <print>

namespace Beer::System
{
    class ColorMixerEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> colorMixerDisplayMat;
        std::vector<UISubEntity> pigmentEntities;
        std::vector<std::shared_ptr<Rendering::Material>> pigmentMaterials;
        std::shared_ptr<Rendering::Texture2D> pigmentTexture;

    public:
        ColorMixerEntity();
        Rendering::Material* GetColorMixerMat() const { return colorMixerDisplayMat.get(); }
        ColorMixerManager* GetMixerManager() const { return static_cast<ColorMixerManager*>(manager.get()); }

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<ColorMixerManager>();
            InitializePigments();
            MarkDirty();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, colorMixerDisplayMat.get()));

            for (int i = 0; i < pigmentEntities.size(); i++)
            {
                if (i >= pigmentMaterials.size())
                    break;

                UISubEntity& subEntity = pigmentEntities[i];
                auto& material = pigmentMaterials[i];
                renderItems.push_back(UIRenderItem(subEntity.GetTransform(), material.get()));
            }

            return renderItems;
        }

        void InitializePigments();
    };
} // namespace Beer::System
