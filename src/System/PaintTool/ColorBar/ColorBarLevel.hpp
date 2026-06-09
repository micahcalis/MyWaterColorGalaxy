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
        BeerEvent<void(glm::vec4, ColorBarLevel)> OnNewColor;

    private:
        std::unique_ptr<Button> button = nullptr;
        Rendering::Material* material = nullptr;
        glm::vec4 color;
        ColorBarLevel level;

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

        glm::vec4 GetColor() const { return color; }

        void SetColor(glm::vec4 color)
        {
            this->color = color;
            material->SetColor("_TintColor", color);
        }

        [[nodiscard]] UITransform* GetTransform() const { return button->GetTransform(); }

    private:
        void ButtonClicked()
        {
            OnButtonClicked.Invoke(this);
        }

        void ColorPickerCallback(glm::vec4 newColor)
        {
            color = newColor;
            material->SetColor("_TintColor", newColor);
            OnNewColor.Invoke(newColor, level);
        }
    };
} // namespace Beer::System
