#pragma once

#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "ToolBarManager.hpp"
#include <memory>

namespace Beer::System
{
    class ToolBarEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> backgroundMat = nullptr;
        std::shared_ptr<Rendering::Texture2D> squareTexture = nullptr;

        std::vector<std::unique_ptr<UISubEntity>> brushes;
        std::vector<std::shared_ptr<Rendering::Material>> brushMaterials;
        std::vector<std::shared_ptr<Rendering::Texture2D>> brushTextures;

        std::vector<std::unique_ptr<UISubEntity>> historyButtons;
        std::vector<std::shared_ptr<Rendering::Material>> historyMaterials;
        std::vector<std::shared_ptr<Rendering::Texture2D>> historyTextures;

        Function<void, GalaxyBrushType> setBrushType = nullptr;

    public:
        ToolBarEntity(Function<void, GalaxyBrushType> setBrushType);

        void Update() override
        {
            QuadTreeEntity::Update();
        }

        void InitializeButtons()
        {
            InitializeBrushes();
            InitializeHistoryButtons();
            MarkDirty();
        }

        [[nodiscard]] ToolBarManager* GetToolBarManager() const { return static_cast<ToolBarManager*>(manager.get()); }

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<ToolBarManager>(setBrushType);
            MarkDirty();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMat.get()));

            for (int i = 0; i < brushes.size(); i++)
            {
                renderItems.push_back(UIRenderItem(brushes[i]->GetTransform(), brushMaterials[i].get()));
            }

            return renderItems;
        }

        void InitializeBrushes();
        void InitializeHistoryButtons();
    };
} // namespace Beer::System
