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
        Function<void, Function<void, ImagePixelData>> subscribeToReadback = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        std::unique_ptr<Button> colorPickerButton = nullptr;
        Rendering::Material* colorPickerButtonMaterial = nullptr;
        std::unique_ptr<Button> paintPigmentButton = nullptr;
        Rendering::Material* paintPigmentButtonMaterial = nullptr;
        UITransform* canvasTransform = nullptr;
        ColorPickingState state = ColorPickingState::Idle;
        glm::vec2 currentPickingPos = glm::vec2(0);
        glm::vec4 pickedColor = glm::vec4(1, 0, 0, 1);

    public:
        ColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
            Function<MouseInput> getMouseInput,
            UITransform* colorPickerTransform,
            Rendering::Material* colorPickerButtonMaterial,
            UITransform* canvasTransform,
            UITransform* paintPigmentTransform,
            Rendering::Material* paintPigmentMaterial)
            : subscribeToReadback(subscribeToReadback), getMouseInput(getMouseInput), canvasTransform(canvasTransform), colorPickerButtonMaterial(colorPickerButtonMaterial), paintPigmentButtonMaterial(paintPigmentMaterial)
        {
            colorPickerButton = std::make_unique<Button>(colorPickerTransform, colorPickerButtonMaterial);
            colorPickerButton->SetOnClick([this]() -> void { ClickSelectColorButton(); });

            paintPigmentButton = std::make_unique<Button>(paintPigmentTransform, paintPigmentMaterial);
            paintPigmentButton->SetOnClick([this]() -> void { ClickPaintPigmentButton(); });
        }

        void Update();
        ColorPickingState GetState() const { return state; }

    private:
        void ClickSelectColorButton();
        void ClickPaintPigmentButton();
        void TryPickColor();
        void SelectColorReadback(ImagePixelData imageData);
        glm::vec2 GetNormalizedCanvasPos(const PixelRect& rect, const glm::vec2 mousePos) const;
        void UpdateButtonMaterials();
    };
} // namespace Beer::System
