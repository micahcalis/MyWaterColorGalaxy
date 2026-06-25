#pragma once

#include "BrushSizeBarManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"
#include <memory>

namespace Beer::System
{
    class BrushSizeBarEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> backgroundMaterial = nullptr;

        std::shared_ptr<Rendering::Texture2D> sliderBarTexture = nullptr;
        std::shared_ptr<Rendering::Texture2D> sliderButtonTexture = nullptr;
        std::unique_ptr<UISubEntity> sliderColliderEntity = nullptr;

        std::shared_ptr<Rendering::Material> sliderButtonMaterial = nullptr;
        std::unique_ptr<UISubEntity> sliderButtonEntity = nullptr;
        std::shared_ptr<Rendering::Material> sliderBarMaterial = nullptr;
        std::unique_ptr<UISubEntity> sliderBarEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> brushSizeTextureBig = nullptr;
        std::shared_ptr<Rendering::Texture2D> brushSizeTextureSmall = nullptr;
        std::shared_ptr<Rendering::Material> sizeIconMaterialBig = nullptr;
        std::shared_ptr<Rendering::Material> sizeIconMaterialSmall = nullptr;
        std::unique_ptr<UISubEntity> smallSizeEntity = nullptr;
        std::unique_ptr<UISubEntity> bigSizeEntity = nullptr;

        Function<void, float> setBrushSize = nullptr;

    public:
        BrushSizeBarEntity(Function<void, float> setBrushSize);

        void Update() override
        {
            QuadTreeEntity::Update();
        }

        void InitializeSlider(UITransform* parent);

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

            if (GetBrushSizeBarManager()->SliderInitialized())
            {
                renderItems.push_back(UIRenderItem(sliderButtonEntity->GetTransform(), sliderButtonMaterial.get()));
                renderItems.push_back(UIRenderItem(sliderBarEntity->GetTransform(), sliderBarMaterial.get()));
                renderItems.push_back(UIRenderItem(smallSizeEntity->GetTransform(), sizeIconMaterialSmall.get()));
                renderItems.push_back(UIRenderItem(bigSizeEntity->GetTransform(), sizeIconMaterialBig.get()));
            }

            return renderItems;
        }

        void InitializeHelpButton();
    };
} // namespace Beer::System
