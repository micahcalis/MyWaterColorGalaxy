#pragma once

#include "ColorBarLevel.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <memory>
#include <unordered_map>

namespace Beer::System
{
    enum class ColorBarType
    {
        Planet,
        Galaxy
    };

    class ColorBarManager : public IEntityManager
    {
    public:
        BeerEvent<void(glm::vec4 color)> OnColorClicked;

    private:
        UITransform* colorBarTransform = nullptr;
        UITransform* selectSpriteTransform = nullptr;
        std::unordered_map<ColorBarLevel, std::unique_ptr<ColorBarController>> planetColorControllers;
        std::unordered_map<ColorBarLevel, std::unique_ptr<ColorBarController>> galaxyColorControllers;
        Rendering::Material* planetDisplayMaterial = nullptr;
        Rendering::Material* galaxyDisplayMaterial = nullptr;

        Function<MouseInput> getMouseInput = nullptr;
        Function<void> markQuadTreeDirty = nullptr;
        Function<void, glm::vec4> setColorDisplayColor = nullptr;
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor = nullptr;
        Function<std::array<glm::vec4, 4>> getGalaxyColors = nullptr;
        Function<Rendering::Texture2D*> getBrushTexture = nullptr;
        Function<void> pickerSelectColor = nullptr;

        ColorBarController* currentController = nullptr;

        BeerEvent<void(glm::vec4)>* onColorPicked = nullptr;
        BeerEvent<void()>* onColorPickerClosed = nullptr;
        BeerEvent<void()>* onNewSeed = nullptr;

    public:
        ColorBarManager(UITransform* colorBarTransform,
            Function<MouseInput> getMouseInput,
            Function<void> markQuadTreeDirty,
            Function<void, glm::vec4> setColorDisplayColor,
            Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor,
            Function<std::array<glm::vec4, 4>> getGalaxyColors,
            Function<Rendering::Texture2D*> getBrushTexture,
            Function<void> pickerSelectColor,
            BeerEvent<void()>* onColorPickerClosed,
            BeerEvent<void()>* onNewSeed);

        void Update() override;

        void SetDisplayMaterials(Rendering::Material* planetDisplayMaterial,
            Rendering::Material* galaxyDisplayMaterial)
        {
            this->planetDisplayMaterial = planetDisplayMaterial;
            this->galaxyDisplayMaterial = galaxyDisplayMaterial;
        }

        void CreateColorBarController(ColorBarLevel level,
            ColorBarType type,
            UITransform* transform,
            Rendering::Material* material,
            glm::vec4 initialColor);

        void ForceSetColorsFromSeed();
        void TrySetColor(glm::vec4 color);

        void ReloadFromSerialized(const SerializablePaintTool& serializedPaintTool);
        void UpdateDisplayMaterials();

        glm::vec4 GetBarColor(ColorBarLevel level, ColorBarType type) const;
        std::vector<glm::vec4> GetColors(ColorBarType type) const;

        void SetSelectSpriteTransform(UITransform* spriteTransform)
        {
            this->selectSpriteTransform = spriteTransform;
        }

        void DeselectColors()
        {
            UITransform* selectParent = selectSpriteTransform->Parent;

            if (selectParent == nullptr)
            {
                return;
            }

            selectParent->UnbindChild(selectSpriteTransform);
            markQuadTreeDirty();
        }

    private:
        bool MouseInContainer(glm::vec2 mousePos);
        void OnColorBarButtonPressed(ColorBarController* controller);
        void SetGalaxyColorsFromSeed();
    };
} // namespace Beer::System
