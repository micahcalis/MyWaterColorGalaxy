#pragma once

#include "Rendering/Texture/Texture2D.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyComponent.hpp"
#include "ToolBarManager.hpp"
#include <memory>
#include <print>
#include <stdexcept>

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

        std::unique_ptr<ColorDisplaySubEntity> colorDisplaySubEntity = nullptr;

        Function<void, GalaxyBrushType> setBrushType = nullptr;

    public:
        ToolBarEntity(Function<void, GalaxyBrushType> setBrushType);

        void Update() override
        {
            QuadTreeEntity::Update();
        }

        void InitializeButtons(Function<uint32_t, const GalaxyComponentData&> addComponent,
            Function<void, uint32_t> eraseComponent)
        {
            InitializeBrushes();
            InitializeHistoryButtons(addComponent, eraseComponent);
            InitializeColorDisplay();
            MarkDirty();
        }

        void BindHistoryActions(BeerEvent<void(uint32_t, const GalaxyComponentData&, bool fromHistory)>* placeEvent,
            BeerEvent<void(uint32_t, const GalaxyComponentData&, bool fromHistory)>* eraseEvent)
        {
            MapHistoryController* historyController = GetToolBarManager()->GetHistoryController();

            if (GetToolBarManager()->GetHistoryController() == nullptr)
            {
                throw std::runtime_error("Trying to bind History Actions but History Controller is null!");
            }

            placeEvent->Subscribe([this](int32_t index, const GalaxyComponentData& data, bool fromHistory) -> void {
                if (fromHistory)
                    return;

                GetToolBarManager()->GetHistoryController()->AddPlaceAction(index, data);
            });

            eraseEvent->Subscribe([this](int32_t index, const GalaxyComponentData& data, bool fromHistory) -> void {
                if (fromHistory)
                    return;

                GetToolBarManager()->GetHistoryController()->AddEraseAction(index, data);
            });
        }

        [[nodiscard]] ToolBarManager* GetToolBarManager() const
        {
            return static_cast<ToolBarManager*>(manager.get());
        }

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

            for (int i = 0; i < historyButtons.size(); i++)
            {
                renderItems.push_back(UIRenderItem(historyButtons[i]->GetTransform(), historyMaterials[i].get()));
            }

            if (colorDisplaySubEntity != nullptr)
            {
                renderItems.append_range(colorDisplaySubEntity->GetRenderItems());
            }

            return renderItems;
        }

        void InitializeBrushes();
        void InitializeHistoryButtons(Function<uint32_t, const GalaxyComponentData&> addComponent,
            Function<void, uint32_t> eraseComponent);

        void InitializeColorDisplay();
    };
} // namespace Beer::System
