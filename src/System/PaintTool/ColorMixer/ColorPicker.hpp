#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Readback/ImagePixelData.hpp"

namespace Beer::System
{
    enum class ColorPickingState
    {
        Idle,
        Picking,
        Waiting
    };

    class ColorPicker
    {
    public:
        BeerEvent<void(glm::vec4)> OnColorPicked;

    private:
        Function<void, Function<void, ImagePixelData>> subscribeToReadback;
        Function<MouseInput> getMouseInput;
        std::unique_ptr<Button> colorPickerButton;
        UITransform* canvasTransform;
        ColorPickingState state = ColorPickingState::Idle;
        glm::vec2 currentPickingPos = glm::vec2(0);
        glm::vec4 pickedColor = glm::vec4(1, 0, 0, 1);

    public:
        ColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
            Function<MouseInput> getMouseInput,
            UITransform* buttonTransform,
            Rendering::Material* buttonMaterial,
            UITransform* canvasTransform)
            : subscribeToReadback(subscribeToReadback), getMouseInput(getMouseInput), canvasTransform(canvasTransform)
        {
            colorPickerButton = std::make_unique<Button>(buttonTransform, buttonMaterial);
            colorPickerButton->SetOnClick([this]() -> void { ClickButton(); });
        }

        void Update();
        ColorPickingState GetState() const { return state; }

    private:
        void ClickButton();
        void TryPickColor();
        void SelectColorReadback(ImagePixelData imageData);
        glm::vec2 GetNormalizedCanvasPos(const PixelRect& rect, const glm::vec2 mousePos) const;
    };
} // namespace Beer::System
