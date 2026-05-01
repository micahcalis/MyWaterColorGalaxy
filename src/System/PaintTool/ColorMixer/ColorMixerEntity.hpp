#pragma once

#include "ColorMixerManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/Delegates/Delegate.hpp"
#include <print>

namespace Beer::System
{
    class ColorMixerEntity : public QuadTreeEntity
    {
    public:
        BeerEvent<void()> OnColorMixerClosed;

    private:
        std::shared_ptr<Rendering::Material> colorMixerDisplayMat;

        std::vector<std::unique_ptr<UISubEntity>> pigmentEntities;
        std::vector<std::shared_ptr<Rendering::Material>> pigmentMaterials;
        std::shared_ptr<Rendering::Texture2D> squareTexture = nullptr;
        std::shared_ptr<Rendering::Texture2D> circleTexture = nullptr;

        std::unique_ptr<UISubEntity> clearButtonIcon = nullptr;
        std::shared_ptr<Rendering::Material> clearIconMaterial = nullptr;
        std::unique_ptr<UISubEntity> clearButtonBg = nullptr;
        std::shared_ptr<Rendering::Material> clearBgMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> clearIconTexture = nullptr;

        std::unique_ptr<UISubEntity> colorPickerIcon = nullptr;
        std::shared_ptr<Rendering::Material> colorPickerIconMaterial = nullptr;
        std::unique_ptr<UISubEntity> colorPickerBg = nullptr;
        std::shared_ptr<Rendering::Material> colorPickerBgMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> colorPickerIconTexture = nullptr;

        std::unique_ptr<UISubEntity> colorDisplay = nullptr;
        std::shared_ptr<Rendering::Material> colorDisplayMaterial = nullptr;

        std::unique_ptr<UISubEntity> closeButton = nullptr;
        std::shared_ptr<Rendering::Material> closeButtonMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> closeButtonTexture = nullptr;

    public:
        ColorMixerEntity();
        Rendering::Material* GetColorMixerMat() const { return colorMixerDisplayMat.get(); }
        ColorMixerManager* GetMixerManager() const { return static_cast<ColorMixerManager*>(manager.get()); }
        void InitializeClearButton(Function<void> markCanvasClear);

        void InitializeColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
            Function<MouseInput> getMouseInput);

        void InitializeCloseButton();

        void Update() override
        {
            QuadTreeEntity::Update();
            manager->Update();
        }

        void Open()
        {
            SetTreeEnabled(true);
        }

        void Close()
        {
            if (GetEnabled())
            {
                SetTreeEnabled(false);
                OnColorMixerClosed.Invoke();
            }
        }

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<ColorMixerManager>();
            InitializePigments();
            MarkDirty();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, colorMixerDisplayMat.get()));

            for (int i = 0; i < pigmentEntities.size(); i++)
            {
                if (i >= pigmentMaterials.size())
                    break;

                auto& subEntity = pigmentEntities[i];
                auto& material = pigmentMaterials[i];
                renderItems.push_back(UIRenderItem(subEntity.get(), material.get()));
            }

            if (GetMixerManager()->ClearButtonInitialized())
            {
                renderItems.push_back(UIRenderItem(clearButtonIcon.get(), clearIconMaterial.get()));
                renderItems.push_back(UIRenderItem(clearButtonBg.get(), clearBgMaterial.get()));
            }

            if (GetMixerManager()->ColorPickerInitialized())
            {
                renderItems.push_back(UIRenderItem(colorPickerIcon.get(), colorPickerIconMaterial.get()));
                renderItems.push_back(UIRenderItem(colorPickerBg.get(), colorPickerBgMaterial.get()));
                renderItems.push_back(UIRenderItem(colorDisplay.get(), colorDisplayMaterial.get()));
            }

            if (GetMixerManager()->CloseButtonInitialized())
            {
                renderItems.push_back(UIRenderItem(closeButton->GetTransform(), closeButtonMaterial.get()));
            }

            return renderItems;
        }

        void InitializePigments();
    };
} // namespace Beer::System
