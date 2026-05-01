#pragma once

#include "ColorBarLevel.hpp"
#include "Rendering/Material/Material.hpp"
#include "StackAnimator.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>
#include <unordered_map>

namespace Beer::System
{
    class ColorBarManager : public IEntityManager
    {
    private:
        UITransform* colorBarTransform = nullptr;
        std::unordered_map<ColorBarLevel, std::unique_ptr<ColorBarController>> colorBarLevels;
        Function<MouseInput> getMouseInput = nullptr;
        Function<void> markQuadTreeDirty = nullptr;
        Function<void> openColorPicker = nullptr;
        Function<void, glm::vec4> setColorDisplayColor = nullptr;
        ColorBarController* currentController = nullptr;
        std::unique_ptr<StackAnimator> colorLayersAnimator = nullptr;

        BeerEvent<void(glm::vec4)>* onColorPicked;
        BeerEvent<void()>* onColorPickerClosed;

    public:
        ColorBarManager(UITransform* colorBarTransform,
            Function<MouseInput> getMouseInput,
            Function<void> markQuadTreeDirty,
            Function<void> openColorPicker,
            Function<void, glm::vec4> setColorDisplayColor,
            BeerEvent<void(glm::vec4)>* onColorPicked,
            BeerEvent<void()>* onColorPickerClosed);

        void Update() override;

        void CreateColorBarController(ColorBarLevel level,
            UITransform* transform,
            Rendering::Material* material,
            glm::vec4 initialColor);

        void CreateAnimator();

        glm::vec4 GetBarColor(ColorBarLevel level) const;

    private:
        bool MouseInContainer(glm::vec2 mousePos);
        void AnimateColorLevels();
        void TryOpenColorPicker(ColorBarController* controller);
    };
} // namespace Beer::System
