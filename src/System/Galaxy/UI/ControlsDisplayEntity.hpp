#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    class ControlsDisplayEntity : public QuadTreeEntity
    {
    private:
        std::unique_ptr<UISubEntity> boostDisplayEntity = nullptr;
        std::shared_ptr<Rendering::Material> boostDisplayMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> boostDisplayTexture = nullptr;

        std::unique_ptr<UISubEntity> steerDisplayEntity = nullptr;
        std::shared_ptr<Rendering::Material> steerDisplayMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> steerDisplayTexture = nullptr;

    public:
        ControlsDisplayEntity();

    protected:
        void InitializeManager() override
        {
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(boostDisplayEntity.get(), boostDisplayMaterial.get()));
            renderItems.push_back(UIRenderItem(steerDisplayEntity.get(), steerDisplayMaterial.get()));
            return renderItems;
        }

        void InitializeDisplays();
    };
} // namespace Beer::System
