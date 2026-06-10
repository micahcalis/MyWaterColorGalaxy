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

    enum class ColorBarType
    {
        Planet,
        Galaxy
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
        ColorBarType type;

    public:
        ColorBarController(UITransform* transform,
            Rendering::Material* material,
            glm::vec4 startColor,
            ColorBarLevel level,
            ColorBarType type)
            : color(startColor)
            , level(level)
            , type(type)
            , material(material)
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
        ColorBarLevel GetLevel() const { return level; }
        ColorBarType GetType() const { return type; }

    private:
        void ButtonClicked()
        {
            OnButtonClicked.Invoke(this);
        }
    };
} // namespace Beer::System
