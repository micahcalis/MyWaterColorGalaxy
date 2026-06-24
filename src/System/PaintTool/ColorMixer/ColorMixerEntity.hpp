#pragma once

#include "ColorMixerManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"
#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"
#include <memory>

namespace Beer::System
{
    class ColorMixerEntity : public QuadTreeEntity
    {
    public:
        BeerEvent<void()> OnColorMixerClosed;

    private:
        Function<MouseInput> getMouseInput = nullptr;
        std::shared_ptr<Rendering::Material> backgroundMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> backgroundTexture = nullptr;

        std::unique_ptr<UISubEntity> colorMixerDisplayEntity = nullptr;
        std::shared_ptr<Rendering::Material> colorMixerDisplayMat = nullptr;
        std::shared_ptr<Rendering::Texture2D> colorMixerDisplayTexture = nullptr;

        std::unique_ptr<UISubEntity> colormixerBorderEntity = nullptr;
        std::shared_ptr<Rendering::Texture2D> colorMixerBorderTexture = nullptr;
        std::shared_ptr<Rendering::Material> colorMixerBorderMaterial = nullptr;

        std::vector<std::unique_ptr<UISubEntity>> pigmentEntities;
        std::vector<std::shared_ptr<Rendering::Material>> pigmentMaterials;
        std::vector<std::shared_ptr<Rendering::Texture2D>> pigmentTextures;

        std::unique_ptr<UISubEntity> clearButtonIcon = nullptr;
        std::shared_ptr<Rendering::Material> clearIconMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> clearIconTexture = nullptr;

        std::unique_ptr<UISubEntity> pigmentsBgEntity = nullptr;
        std::shared_ptr<Rendering::Material> pigmentsBgMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> pigmentsBgTex = nullptr;

        std::unique_ptr<UISubEntity> selectSpriteEntity = nullptr;
        std::shared_ptr<Rendering::Material> selectSpriteMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> selectSpriteTexture = nullptr;

        std::unique_ptr<UISubEntity> cursorSpriteEntity = nullptr;
        std::unique_ptr<UISubEntity> cursorAnchorEntity = nullptr;
        std::shared_ptr<Rendering::Texture2D> pickerSprite = nullptr;
        std::shared_ptr<Rendering::Texture2D> pickerSpriteMask = nullptr;
        std::shared_ptr<Rendering::Texture2D> brushSprite = nullptr;
        std::shared_ptr<Rendering::Texture2D> brushSpriteMask = nullptr;
        std::shared_ptr<Rendering::Material> cursorSpriteMaterial = nullptr;

        std::unique_ptr<HelpButtonSubEntity> helpButtonSubEntity = nullptr;
        std::unique_ptr<ColorDisplaySubEntity> colorDisplaySubEntity = nullptr;

    public:
        ColorMixerEntity(Function<MouseInput> getMouseInput);
        Rendering::Material* GetColorMixerMat() const { return colorMixerDisplayMat.get(); }
        ColorMixerManager* GetMixerManager() const { return static_cast<ColorMixerManager*>(manager.get()); }
        void InitializeClearButton(Function<void> markCanvasClear);

        void InitializeColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
            Function<MouseInput> getMouseInput);

        void Update() override
        {
            QuadTreeEntity::Update();
            manager->Update();
        }

        void Open()
        {
            if (!GetEnabled())
            {
                SetTreeEnabled(true);
            }
        }

        void Close()
        {
            if (GetEnabled())
            {
                SetTreeEnabled(false);
                OnColorMixerClosed.Invoke();
            }
        }

        void SetColorDisplayColor(glm::vec4 displayColor)
        {
            if (!GetMixerManager()->ColorPickerInitialized())
                return;
        }

        UITransform* GetMixerTransform() const { return colorMixerDisplayEntity->GetTransform(); }

    protected:
        void InitializeManager() override
        {
            Function<void> markDirty = [this]() -> void { MarkDirty(); };

            manager = std::make_unique<ColorMixerManager>(colorMixerDisplayEntity->GetTransform(),
                markDirty,
                getMouseInput);

            InitializePigments();
            MarkDirty();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMaterial.get()));
            renderItems.push_back(UIRenderItem(colorMixerDisplayEntity->GetTransform(), colorMixerDisplayMat.get()));
            renderItems.push_back(UIRenderItem(colormixerBorderEntity->GetTransform(), colorMixerBorderMaterial.get()));

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
            }

            if (selectSpriteEntity->GetTransform()->Parent != nullptr)
            {
                renderItems.push_back(UIRenderItem(selectSpriteEntity->GetTransform(), selectSpriteMaterial.get()));
            }

            if (pigmentsBgEntity != nullptr)
            {
                renderItems.push_back(UIRenderItem(pigmentsBgEntity->GetTransform(), pigmentsBgMaterial.get()));
            }

            if (GetMixerManager()->GetMixerCursor() != nullptr)
            {
                renderItems.push_back(UIRenderItem(cursorSpriteEntity->GetTransform(), cursorSpriteMaterial.get()));
            }

            if (helpButtonSubEntity != nullptr)
            {
                renderItems.append_range(helpButtonSubEntity->GetRenderItems());
            }

            if (colorDisplaySubEntity != nullptr)
            {
                renderItems.append_range(colorDisplaySubEntity->GetRenderItems());
            }

            return renderItems;
        }

        void InitializePigments();
        void InitializeSelectSpriteEntity();
        void InitializeCursorSprite();
        void InitializeHelpButton();
        void InitializeColorDisplay();
    };
} // namespace Beer::System
