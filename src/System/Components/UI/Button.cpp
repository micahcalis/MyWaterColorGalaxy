#include "System/Components/UI/Button.hpp"
#include <string>

namespace Beer::System
{
    static const std::string BUTTON_STATE = "_ButtonState";

    void Button::OnHover(MouseInput mouseInput)
    {
        if (mouseInput.LeftClickHold)
        {
            spriteMaterial->SetInt(BUTTON_STATE, static_cast<uint32_t>(ButtonSpriteState::Click));
        } else
        {
            spriteMaterial->SetInt(BUTTON_STATE, static_cast<uint32_t>(ButtonSpriteState::Hover));
        }

        if (mouseInput.leftClickStart)
        {
            if (onClick != nullptr)
            {
                onClick();
            }
        }
    }

    void Button::OnHoverExit()
    {
        spriteMaterial->SetInt(BUTTON_STATE, static_cast<uint32_t>(ButtonSpriteState::Default));
    }
} // namespace Beer::System
