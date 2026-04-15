#pragma once

#include "System/Base/Input/MouseInput.hpp"
#include "glm/fwd.hpp"
#include "SDL3/SDL_mouse.h"

namespace Beer::System
{
    class InputManager
    {
    private:
        SDL_MouseButtonFlags previousMouseState = 0;
        SDL_MouseButtonFlags currentMouseState = 0;

    public:
        void Update();
        glm::vec2 GetMovementVector();
        glm::vec2 GetMouseVector();
        glm::vec2 GetMousePosition();
        MouseInput GetMouseInput();
    };
} // namespace Beer::System
