#pragma once

#include "ButtonInput.hpp"
#include "SDL3/SDL_scancode.h"
#include "System/Base/Input/ButtonInput.hpp"
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
        KeyButtonCache debugKeyCache;
        KeyButtonCache spaceKeyCache;
        KeyButtonCache tabKeyCache;
        KeyButtonCache pKeyCache;
        KeyButtonCache escKeyCache;

        float scrollY = 0;

    public:
        InputManager()
            : debugKeyCache(SDL_SCANCODE_O)
            , spaceKeyCache(SDL_SCANCODE_SPACE)
            , tabKeyCache(SDL_SCANCODE_TAB)
            , pKeyCache(SDL_SCANCODE_P)
            , escKeyCache(SDL_SCANCODE_ESCAPE)
        {
        }

        void Update();
        void SetScroll(float scrollY);
        glm::vec2 GetMovementVector();
        glm::vec2 GetMouseVector();
        glm::vec2 GetMousePosition();
        MouseInput GetMouseInput();
        ButtonInput GetDebugButtonInput();
        ButtonInput GetSpaceButtonInput();
        ButtonInput GetTabButtonInput();
        ButtonInput GetPButtonInput();
        ButtonInput GetEscButtonInput();
    };
} // namespace Beer::System
