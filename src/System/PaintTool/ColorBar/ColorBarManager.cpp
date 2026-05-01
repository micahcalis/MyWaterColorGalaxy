#include "System/PaintTool/ColorBar/ColorBarManager.hpp"
#include "ColorBarLevel.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include <stdexcept>

namespace Beer::System
{
    ColorBarManager::ColorBarManager(UITransform* colorBarTransform,
        Function<MouseInput> getMouseInput,
        Function<void> markQuadTreeDirty,
        Function<void> openColorPicker,
        BeerEvent<void(glm::vec4)>* onColorPicked,
        BeerEvent<void()>* onColorPickerClosed)
        : colorBarTransform(colorBarTransform), getMouseInput(getMouseInput), markQuadTreeDirty(markQuadTreeDirty), openColorPicker(openColorPicker), onColorPicked(onColorPicked), onColorPickerClosed(onColorPickerClosed)
    {
        onColorPickerClosed->Subscribe([this]() {
            currentController->Unsubscribe();
            currentController = nullptr;
        });
    }

    void ColorBarManager::Update()
    {
        MouseInput input = getMouseInput();

        if (MouseInContainer(input.PixelPos))
        {
            AnimateColorLevels();
            markQuadTreeDirty();
        }
    }

    void ColorBarManager::CreateColorBarController(ColorBarLevel level,
        UITransform* transform,
        Rendering::Material* material,
        glm::vec4 initialColor)
    {
        if (colorBarLevels.contains(level))
        {
            throw std::runtime_error(std::format("Color Bar Level Already Initialized: {}", magic_enum::enum_name(level)));
        }

        std::unique_ptr<ColorBarController> controller = std::make_unique<ColorBarController>(transform,
            material,
            initialColor,
            level);

        Function<SubscriptionToken, Function<void, glm::vec4>> subscribeToColorPicker =
            [this](Function<void, glm::vec4> func) -> SubscriptionToken {
            return onColorPicked->Subscribe(func);
        };

        Function unsubscribeToColorPicker = [this](SubscriptionToken token) -> void {
            onColorPicked->Unsubscribe(token);
        };

        controller->SetSubscriptions(subscribeToColorPicker, unsubscribeToColorPicker);
        controller->OnButtonClicked.Subscribe([this](ColorBarController* controller) -> void { TryOpenColorPicker(controller); });

        colorBarLevels[level] = std::move(controller);
    }

    glm::vec4 ColorBarManager::GetBarColor(ColorBarLevel level) const
    {
        if (!colorBarLevels.contains(level))
        {
            throw std::runtime_error(std::format("Color Bar Level Unitialized: {}", magic_enum::enum_name(level)));
        }

        return colorBarLevels.at(level)->GetColor();
    }

    bool ColorBarManager::MouseInContainer(glm::vec2 mousePos)
    {
        return QuadCollider::Hit(colorBarTransform, mousePos);
    }

    void ColorBarManager::AnimateColorLevels()
    {
        // TODO: cool animation yur
    }

    void ColorBarManager::TryOpenColorPicker(ColorBarController* controller)
    {
        if (currentController != nullptr)
        {
            currentController->Unsubscribe();
        }

        currentController = controller;
        openColorPicker();
    }
} // namespace Beer::System

