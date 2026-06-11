#pragma once

#include "MenuBarManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include <memory>
namespace Beer::System
{
    class MenuBarEntity : public QuadTreeEntity
    {
    private:
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;

        std::shared_ptr<Rendering::Material> backgroundMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> backgroundTexture = nullptr;

        std::shared_ptr<Rendering::Material> seedButtonMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> seedButtonTexture = nullptr;
        std::unique_ptr<UISubEntity> seedButtonEntity = nullptr;

        std::shared_ptr<Rendering::Material> flyButtonMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> flyButtonTexture = nullptr;
        std::unique_ptr<UISubEntity> flyButtonEntity = nullptr;

        std::shared_ptr<Rendering::Material> backButtonMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> backButtonTexture = nullptr;
        std::unique_ptr<UISubEntity> backButtonEntity = nullptr;

        Function<void> clearHistory = nullptr;
        Function<void> saveMap = nullptr;
        Function<void> onBackToTitle = nullptr;

    public:
        MenuBarEntity(GalaxyMapBuffer* galaxyMapBuffer,
            Function<void> clearHistory,
            Function<void> saveMap,
            Function<void> onBackToTitle);

        void InitializeButtonEntities();
        MenuBarManager* GetMenuBarManager() const { return static_cast<MenuBarManager*>(manager.get()); };

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<MenuBarManager>(galaxyMapBuffer,
                clearHistory,
                saveMap,
                onBackToTitle);
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMaterial.get()));
            renderItems.push_back(UIRenderItem(seedButtonEntity->GetTransform(), seedButtonMaterial.get()));
            renderItems.push_back(UIRenderItem(flyButtonEntity->GetTransform(), flyButtonMaterial.get()));
            renderItems.push_back(UIRenderItem(backButtonEntity->GetTransform(), backButtonMaterial.get()));
            return renderItems;
        }
    };
} // namespace Beer::System
