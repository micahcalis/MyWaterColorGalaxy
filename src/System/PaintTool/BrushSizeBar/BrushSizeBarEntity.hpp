#pragma once

#include "BrushSizeBarManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include <memory>

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
        std::shared_ptr<Rendering::Material> sliderBarMaterial = nullptr;
        std::unique_ptr<UISubEntity> sliderBarEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> brushSizeTexture = nullptr;
        std::shared_ptr<Rendering::Material> sizeIconMaterial = nullptr;
        std::unique_ptr<UISubEntity> smallSizeEntity = nullptr;
        std::unique_ptr<UISubEntity> bigSizeEntity = nullptr;

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
                renderItems.push_back(UIRenderItem(sliderBarEntity->GetTransform(), sliderBarMaterial.get()));
                renderItems.push_back(UIRenderItem(smallSizeEntity->GetTransform(), sizeIconMaterial.get()));
                renderItems.push_back(UIRenderItem(bigSizeEntity->GetTransform(), sizeIconMaterial.get()));
            }

            return renderItems;
        }
    };
} // namespace Beer::System
