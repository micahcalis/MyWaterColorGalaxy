#pragma once

#include "BrushSizeBarManager.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    class BrushSizeBarEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> backgroundMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> squareTexture = nullptr;

        std::unique_ptr<UISubEntity> sliderColliderEntity = nullptr;

        std::shared_ptr<Rendering::Material> sliderButtonMaterial = nullptr;
        std::unique_ptr<UISubEntity> sliderButtonEntity = nullptr;

        Function<void, float> setBrushSize = nullptr;

    public:
        BrushSizeBarEntity(Function<void, float> setBrushSize);

        void Update() override
        {
            QuadTreeEntity::Update();
        }

        void InitializeSlider();

        [[nodiscard]] BrushSizeBarManager* GetBrushSizeBarManager() const { return static_cast<BrushSizeBarManager*>(manager.get()); }

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<BrushSizeBarManager>([this](float size) -> void {
                setBrushSize(size);
                MarkDirty();
            });

            MarkDirty();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMaterial.get()));

            if (GetBrushSizeBarManager()->SliderInitialized())
            {
                renderItems.push_back(UIRenderItem(sliderButtonEntity->GetTransform(), sliderButtonMaterial.get()));
            }

            return renderItems;
        }
    };
} // namespace Beer::System
