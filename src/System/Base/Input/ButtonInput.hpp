#pragma once

#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_scancode.h"

namespace Beer::System
{
    struct ButtonInput
    {
    public:
        bool ButtonStart;
        bool ButtonHold;
        bool ButtonExit;
    };

    class KeyButtonCache
    {
    private:
        SDL_Scancode scanCode;
        bool wasDown = false;

    public:
        KeyButtonCache(SDL_Scancode scanCode)
            : scanCode(scanCode)
        {
        }

        ButtonInput UpdateInput()
        {
            const bool* keyboardState = SDL_GetKeyboardState(nullptr);

            bool isDown = keyboardState[scanCode];

            ButtonInput input;
            input.ButtonStart = isDown && !wasDown;
            input.ButtonHold = isDown;
            input.ButtonExit = !isDown && wasDown;
            wasDown = isDown;

            return input;
        }
    };
} // namespace Beer::System
