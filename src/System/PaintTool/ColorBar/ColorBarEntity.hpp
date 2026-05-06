#pragma once

#include "ColorBarManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include <print>

namespace Beer::System
{
    class ColorBarEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> backgroundMat = nullptr;
        std::shared_ptr<Rendering::Texture2D> squareTexture = nullptr;
        std::vector<std::unique_ptr<UISubEntity>> colorLayers;
        std::vector<std::shared_ptr<Rendering::Material>> colorLayerMaterials;
        std::shared_ptr<Rendering::Texture2D> colorLayerSprite = nullptr;

        Function<MouseInput> getMouseInput = nullptr;
        Function<void> markQuadTreeDirty = nullptr;
        Function<void> openColorPicker = nullptr;
        Function<void, glm::vec4> setColorDisplayColor = nullptr;
        BeerEvent<void(glm::vec4)>* onColorPicked = nullptr;
        BeerEvent<void()>* onColorPickerClosed = nullptr;

    public:
        ColorBarEntity(Function<MouseInput> getMouseInput,
            Function<void> openColorPicker,
            Function<void, glm::vec4> setColorDisplayColor,
            BeerEvent<void(glm::vec4)>* onColorPicked,
            BeerEvent<void()>* onColorPickerClosed);

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
                openColorPicker,
                setColorDisplayColor,
                onColorPicked,
                onColorPickerClosed);
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, backgroundMat.get()));

            for (int i = 0; i < colorLayers.size(); i++)
            {
                renderItems.push_back(UIRenderItem(colorLayers[i].get(), colorLayerMaterials[i].get()));
            }

            return renderItems;
        }
    };
} // namespace Beer::System
