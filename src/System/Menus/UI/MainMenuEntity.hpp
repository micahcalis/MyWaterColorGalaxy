#pragma once

#include "System/Menus/UI/MainMenuManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include <vector>

namespace Beer::System
{
    class MainMenuEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Texture2D> logoTexture = nullptr;
        std::shared_ptr<Rendering::Material> logoMaterial = nullptr;
        std::unique_ptr<UISubEntity> logoEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> startTexture = nullptr;
        std::shared_ptr<Rendering::Material> startMaterial = nullptr;
        std::unique_ptr<UISubEntity> startEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> exitTexture = nullptr;
        std::shared_ptr<Rendering::Material> exitMaterial = nullptr;
        std::unique_ptr<UISubEntity> exitEntity = nullptr;

    public:
        MainMenuEntity();
        void InitializeButtons();
        MainMenuManager* GetMainMenuManager() const { return static_cast<MainMenuManager*>(manager.get()); };

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<MainMenuManager>();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(logoEntity->GetTransform(), logoMaterial.get()));
            renderItems.push_back(UIRenderItem(startEntity->GetTransform(), startMaterial.get()));
            renderItems.push_back(UIRenderItem(exitEntity->GetTransform(), exitMaterial.get()));
            return renderItems;
        }
    };
} // namespace Beer::System
