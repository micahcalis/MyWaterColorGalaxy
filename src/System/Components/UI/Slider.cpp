#include "System/Components/UI/Slider.hpp"
#include "System/Components/UI/Button.hpp"
#include "glm/fwd.hpp"
#include <algorithm>

namespace Beer::System
{
    static const std::string BUTTON_STATE = "_ButtonState";

    void Slider::OnHover(MouseInput mouseInput)
    {
        if (mouseInput.LeftClickHold)
        {
            spriteMaterial->SetInt(BUTTON_STATE, static_cast<uint32_t>(ButtonSpriteState::Click));
            OnSelect(mouseInput.PixelPos);
            OnSliderChanged.Invoke(value);
        } else
        {
            spriteMaterial->SetInt(BUTTON_STATE, static_cast<uint32_t>(ButtonSpriteState::Hover));
        }
    }

    void Slider::OnHoverExit()
    {
        spriteMaterial->SetInt(BUTTON_STATE, static_cast<uint32_t>(ButtonSpriteState::Default));
    }

    void Slider::OnSelect(glm::vec2 position)
    {
        float axisValue = type == SliderType::Horizontal ? position.x : position.y;

        glm::vec2 bounds = glm::vec2(0);

        if (type == SliderType::Horizontal)
        {
            bounds.x = direction == SliderDirection::Positive
                ? collider->GetTransform()->Rect.BotLeft.x
                : collider->GetTransform()->Rect.TopRight.x;

            bounds.y = direction == SliderDirection::Positive
                ? collider->GetTransform()->Rect.TopRight.x
                : collider->GetTransform()->Rect.BotLeft.x;

        } else if (type == SliderType::Vertical)
        {
            bounds.x = direction == SliderDirection::Positive
                ? collider->GetTransform()->Rect.BotLeft.y
                : collider->GetTransform()->Rect.TopRight.y;

            bounds.y = direction == SliderDirection::Positive
                ? collider->GetTransform()->Rect.TopRight.y
                : collider->GetTransform()->Rect.BotLeft.y;
        }

        if (bounds.x == bounds.y)
            return;

        value = (axisValue - bounds.x) / (bounds.y - bounds.x);
        value = std::clamp(value, 0.0f, 1.0f);

        UpdateButtonPosition();
    }

    void Slider::UpdateButtonPosition()
    {
        float normalizedOffset = value - 0.5f;

        if (direction == SliderDirection::Negative)
        {
            normalizedOffset *= -1.0f;
        }

        if (type == SliderType::Horizontal)
        {
            buttonTransform->Position.x = normalizedOffset * collider->GetTransform()->Scale.x;
        } else if (type == SliderType::Vertical)
        {
            buttonTransform->Position.y = normalizedOffset * collider->GetTransform()->Scale.y;
        }
    }
} // namespace Beer::System
