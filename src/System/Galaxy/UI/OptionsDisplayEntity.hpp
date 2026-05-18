#pragma once

#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include <memory>

namespace Beer::System
{
    class OptionsDisplayEntity : public QuadTreeEntity
    {
    private:
        std::unique_ptr<UISubEntity> photoDisplayEntity = nullptr;
        std::shared_ptr<Rendering::Material> photoDisplayMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> photoDisplayTexture = nullptr;

        std::unique_ptr<UISubEntity> returnDisplayEntity = nullptr;
        std::shared_ptr<Rendering::Material> returnDisplayMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> returnDisplayTexture = nullptr;

    public:
        OptionsDisplayEntity();

    protected:
        void InitializeManager() override
        {
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(photoDisplayEntity.get(), photoDisplayMaterial.get()));
            renderItems.push_back(UIRenderItem(returnDisplayEntity.get(), returnDisplayMaterial.get()));
            return renderItems;
        }

        void InitializeDisplays();
    };
} // namespace Beer::System
