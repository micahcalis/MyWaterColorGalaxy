#pragma once

#include "ColorBarManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"
#include <print>

namespace Beer::System
{
    class ColorBarEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> backgroundMat = nullptr;
        std::shared_ptr<Rendering::Texture2D> backgroundTexture = nullptr;
        std::vector<std::unique_ptr<UISubEntity>> colorLayers;
        std::vector<std::shared_ptr<Rendering::Material>> colorLayerMaterials;
        std::shared_ptr<Rendering::Texture2D> colorLayerSprite = nullptr;

        std::unique_ptr<UISubEntity> planetBarBgEntity = nullptr;
        std::shared_ptr<Rendering::Material> planetDisplayMaterial = nullptr;
        std::unique_ptr<UISubEntity> planetDisplayEntity = nullptr;
        std::unique_ptr<UISubEntity> galaxyBarBgEntity = nullptr;
        std::shared_ptr<Rendering::Texture2D> galaxyDisplayTexture = nullptr;
        std::shared_ptr<Rendering::Texture2D> galaxyDisplayMask = nullptr;
        std::shared_ptr<Rendering::Material> galaxyDisplayMaterial = nullptr;
        std::unique_ptr<UISubEntity> galaxyDisplayEntity = nullptr;

        std::shared_ptr<Rendering::Material> selectSpriteMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> selectSpriteTexture = nullptr;
        std::unique_ptr<UISubEntity> selectSpriteEntity = nullptr;

        Function<MouseInput> getMouseInput = nullptr;
        Function<void> markQuadTreeDirty = nullptr;
        Function<void, glm::vec4> setColorDisplayColor = nullptr;
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor = nullptr;
        Function<std::array<glm::vec4, 4>> getGalaxyColors = nullptr;
        Function<Rendering::Texture2D*> getBrushTexture = nullptr;
        Function<Rendering::Texture2D*> getBrushMask = nullptr;
        Function<void> pickerSelectColor = nullptr;
        BeerEvent<void(glm::vec4)>* onColorPicked = nullptr;
        BeerEvent<void()>* onColorPickerClosed = nullptr;
        BeerEvent<void()>* onNewSeed = nullptr;

    public:
        ColorBarEntity(Function<MouseInput> getMouseInput,
            Function<void, glm::vec4> setColorDisplayColor,
            Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor,
            Function<std::array<glm::vec4, 4>> getGalaxyColors,
            Function<Rendering::Texture2D*> getBrushTexture,
            Function<Rendering::Texture2D*> getBrushMask,
            Function<void> pickerSelectColor,
            BeerEvent<void(glm::vec4)>* onColorPicked,
            BeerEvent<void()>* onColorPickerClosed,
            BeerEvent<void()>* onNewSeed);

        void InitializeColorLayers();
        ColorBarManager* GetColorBarManager() const { return static_cast<ColorBarManager*>(manager.get()); }

        void Update() override
        {
            manager->Update();
            QuadTreeEntity::Update();
        }

    protected:
        void InitializeManager() override
        {
            MarkDirty();

            manager = std::make_unique<ColorBarManager>(&rootTransform,
                getMouseInput,
                markQuadTreeDirty,
                setColorDisplayColor,
                setGalaxyBufferColor,
                getGalaxyColors,
                getBrushTexture,
                getBrushMask,
                pickerSelectColor,
                onColorPickerClosed,
                onNewSeed);

            onColorPicked->Subscribe([this](glm::vec4 color) -> void {
                GetColorBarManager()->TrySetColor(color);
            });
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(planetBarBgEntity->GetTransform(), backgroundMat.get()));
            renderItems.push_back(UIRenderItem(galaxyBarBgEntity->GetTransform(), backgroundMat.get()));

            for (int i = 0; i < colorLayers.size(); i++)
            {
                renderItems.push_back(UIRenderItem(colorLayers[i].get(), colorLayerMaterials[i].get()));
            }

            renderItems.push_back(UIRenderItem(planetDisplayEntity->GetTransform(), planetDisplayMaterial.get()));
            renderItems.push_back(UIRenderItem(galaxyDisplayEntity->GetTransform(), galaxyDisplayMaterial.get()));

            if (selectSpriteEntity->GetTransform()->Parent != nullptr)
            {
                renderItems.push_back(UIRenderItem(selectSpriteEntity->GetTransform(), selectSpriteMaterial.get()));
            };

            return renderItems;
        }

    private:
        void InitializeDisplays();
        void InitializeBackgrounds();
        void InitializeSelectSpriteEntity();
    };
} // namespace Beer::System
