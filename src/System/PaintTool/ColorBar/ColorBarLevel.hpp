#pragma once

#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include <memory>

namespace Beer::System
{
    enum class ColorBarLevel : uint32_t
    {
        Primary = 0,
        Secondary = 1,
        Tertiary = 2,
        Quaternary = 3
    };

    class ColorBarController
    {
    public:
        BeerEvent<void(ColorBarController*)> OnButtonClicked;

    private:
        std::unique_ptr<Button> button = nullptr;
        Rendering::Material* material = nullptr;
        glm::vec4 color;
        ColorBarLevel level;
        SubscriptionToken token = INVALID_TOKEN;

        Function<SubscriptionToken, Function<void, glm::vec4>> subscribeToColorPicker = nullptr;
        Function<void, SubscriptionToken> unsubscribeToColorPicker = nullptr;

    public:
        ColorBarController(UITransform* transform,
            Rendering::Material* material,
            glm::vec4 startColor,
            ColorBarLevel level)
            : color(startColor), level(level), material(material)
        {
            button = std::make_unique<Button>(transform, material);
            button->SetOnClick([this]() -> void { ButtonClicked(); });
        }

        void Unsubscribe()
        {
            if (unsubscribeToColorPicker != nullptr)
            {
                unsubscribeToColorPicker(token);
                token = INVALID_TOKEN;
            }
        }

        glm::vec4 GetColor() const { return color; }

        void SetSubscriptions(Function<SubscriptionToken, Function<void, glm::vec4>> subscribeToColorPicker,
            Function<void, SubscriptionToken> unsubscribeToColorPicker)
        {
            this->subscribeToColorPicker = subscribeToColorPicker;
            this->unsubscribeToColorPicker = unsubscribeToColorPicker;
        }

        [[nodiscard]] UITransform* GetTransform() const { return button->GetTransform(); }

    private:
        void ButtonClicked()
        {
            if (subscribeToColorPicker != nullptr && token == INVALID_TOKEN)
            {
                token = subscribeToColorPicker([this](glm::vec4 newColor) -> void { ColorPickerCallback(newColor); });
            }

            OnButtonClicked.Invoke(this);
        }

        void ColorPickerCallback(glm::vec4 newColor)
        {
            color = newColor;
            material->SetColor("_TintColor", newColor);
        }
    };
} // namespace Beer::System
