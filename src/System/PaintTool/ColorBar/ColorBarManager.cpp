#include "System/PaintTool/ColorBar/ColorBarManager.hpp"
#include "ColorBarLevel.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include <cstdlib>
#include <stdexcept>

namespace Beer::System
{
    static const float ANIMATION_SPEED = 3.0f;
    static const float OFFSET_SCALE = 0.05f;

    ColorBarManager::ColorBarManager(UITransform* colorBarTransform,
        Function<MouseInput> getMouseInput,
        Function<void> markQuadTreeDirty,
        Function<void> openColorPicker,
        Function<void, glm::vec4> setColorDisplayColor,
        BeerEvent<void(glm::vec4)>* onColorPicked,
        BeerEvent<void()>* onColorPickerClosed)
        : colorBarTransform(colorBarTransform), getMouseInput(getMouseInput), markQuadTreeDirty(markQuadTreeDirty), openColorPicker(openColorPicker), setColorDisplayColor(setColorDisplayColor), onColorPicked(onColorPicked), onColorPickerClosed(onColorPickerClosed)
    {
        onColorPickerClosed->Subscribe([this]() {
            currentController->Unsubscribe();
            currentController = nullptr;
        });
    }

    void ColorBarManager::Update()
    {
        AnimateColorLevels();
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

    void ColorBarManager::CreateAnimator()
    {
        std::vector<UITransform*> transforms;
        transforms.reserve(colorBarLevels.size());

        for (int i = 0; i < colorBarLevels.size(); i++)
        {
            ColorBarLevel level = static_cast<ColorBarLevel>(i);
            transforms.push_back(colorBarLevels.at(level)->GetTransform());
        }

        colorLayersAnimator = std::make_unique<StackAnimator>(colorBarTransform,
            transforms,
            ANIMATION_SPEED,
            OFFSET_SCALE);
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
        if (colorLayersAnimator == nullptr)
            return;

        MouseInput input = getMouseInput();
        int selectedLayer = -1;

        if (MouseInContainer(input.PixelPos))
        {
            for (int i = static_cast<int>(colorBarLevels.size()) - 1; i >= 0; i--)
            {
                ColorBarLevel levelToCheck = static_cast<ColorBarLevel>(i);
                const auto& controller = colorBarLevels.at(levelToCheck);

                if (QuadCollider::Hit(controller->GetTransform(), input.PixelPos))
                {
                    selectedLayer = i;
                    break;
                }
            }
        }

        colorLayersAnimator->SetSelectedLayer(selectedLayer);
        bool layersDirty;
        colorLayersAnimator->Update(layersDirty);

        if (layersDirty)
            markQuadTreeDirty();
    }

    void ColorBarManager::TryOpenColorPicker(ColorBarController* controller)
    {
        if (currentController != nullptr)
        {
            currentController->Unsubscribe();
        }

        currentController = controller;
        openColorPicker();
        setColorDisplayColor(currentController->GetColor());
    }
} // namespace Beer::System

