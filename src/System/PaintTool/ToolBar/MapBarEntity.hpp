#pragma once

#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include <memory>

namespace Beer::System
{
    static const uint32_t TOOL_COUNT = 3;

    using MapBarInitializationFunc = Function<void,
        std::vector<std::unique_ptr<UISubEntity>>&,
        std::vector<std::shared_ptr<Rendering::Material>>&>;

    class MapBarEntity : public QuadTreeEntity
    {
    public:
        inline static const std::vector<GalaxyBrushType> TOOL_TYPES = {
            GalaxyBrushType::Eraser,
            GalaxyBrushType::Hand,
            GalaxyBrushType::MagnifyingGlass};

    private:
        std::unique_ptr<UISubEntity> backgroundSubEntity = nullptr;
        std::shared_ptr<Rendering::Material> backgroundMat = nullptr;
        std::shared_ptr<Rendering::Texture2D> backgroundTexture = nullptr;

        std::vector<std::unique_ptr<UISubEntity>> toolEntities;
        std::vector<std::shared_ptr<Rendering::Material>> toolMaterials;
        std::vector<std::shared_ptr<Rendering::Texture2D>> toolTextures;

        std::vector<std::unique_ptr<UISubEntity>> historyButtons;
        std::vector<std::shared_ptr<Rendering::Material>> historyMaterials;
        std::vector<std::shared_ptr<Rendering::Texture2D>> historyTextures;

    public:
        MapBarEntity();

        void InitializeButtons(UITransform* parent,
            MapBarInitializationFunc initializeTools,
            MapBarInitializationFunc initializeHistoryButtons);

    protected:
        void InitializeManager() override
        {
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(backgroundSubEntity->GetTransform(), backgroundMat.get()));

            for (int i = 0; i < toolEntities.size(); i++)
            {
                renderItems.push_back(UIRenderItem(toolEntities[i]->GetTransform(), toolMaterials[i].get()));
            }

            for (int i = 0; i < historyButtons.size(); i++)
            {
                renderItems.push_back(UIRenderItem(historyButtons[i]->GetTransform(), historyMaterials[i].get()));
            }

            return renderItems;
        }
    };
} // namespace Beer::System
