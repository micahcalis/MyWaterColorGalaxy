#pragma once

#include "ColorBarLevel.hpp"
#include "Rendering/Material/Material.hpp"
#include "StackAnimator.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
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
    private:
        UITransform* colorBarTransform = nullptr;
        std::unordered_map<ColorBarLevel, std::unique_ptr<ColorBarController>> planetColorControllers;
        std::unordered_map<ColorBarLevel, std::unique_ptr<ColorBarController>> galaxyColorControllers;

        Function<MouseInput> getMouseInput = nullptr;
        Function<void> markQuadTreeDirty = nullptr;
        Function<void> openColorPicker = nullptr;
        Function<void, glm::vec4> setColorDisplayColor = nullptr;
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor = nullptr;
        Function<std::array<glm::vec4, 4>> getGalaxyColors = nullptr;

        ColorBarController* currentController = nullptr;

        std::unique_ptr<StackAnimator> planetLayersAnimator = nullptr;
        std::unique_ptr<StackAnimator> galaxyLayersAnimator = nullptr;

        BeerEvent<void(glm::vec4)>* onColorPicked = nullptr;
        BeerEvent<void()>* onColorPickerClosed = nullptr;
        BeerEvent<void()>* onNewSeed = nullptr;

    public:
        ColorBarManager(UITransform* colorBarTransform,
            Function<MouseInput> getMouseInput,
            Function<void> markQuadTreeDirty,
            Function<void> openColorPicker,
            Function<void, glm::vec4> setColorDisplayColor,
            Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor,
            Function<std::array<glm::vec4, 4>> getGalaxyColors,
            BeerEvent<void(glm::vec4)>* onColorPicked,
            BeerEvent<void()>* onColorPickerClosed,
            BeerEvent<void()>* onNewSeed);

        void Update() override;

        void CreateColorBarController(ColorBarLevel level,
            ColorBarType type,
            UITransform* transform,
            Rendering::Material* material,
            glm::vec4 initialColor);

        void CreateAnimator(ColorBarType type);
        void ForceSetColorsFromSeed();

        void ReloadFromSerialized(const SerializablePaintTool& serializedPaintTool);

        glm::vec4 GetBarColor(ColorBarLevel level, ColorBarType type) const;
        std::vector<glm::vec4> GetColors(ColorBarType type) const;

    private:
        bool MouseInContainer(glm::vec2 mousePos);
        void AnimateColorLevels(ColorBarType type);
        void TryOpenColorPicker(ColorBarController* controller);
        void SetGalaxyColorsFromSeed();
    };
} // namespace Beer::System
