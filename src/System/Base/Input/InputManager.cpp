#pragma once

#include "System/Base/Input/InputManager.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "glm/glm.hpp"

namespace Beer::System
{
    void InputManager::Update()
    {
        previousMouseState = currentMouseState;
        currentMouseState = SDL_GetMouseState(nullptr, nullptr);
    }

    glm::vec2 InputManager::GetMovementVector()
    {
        int keys;
        const bool* keyStates = SDL_GetKeyboardState(&keys);
        glm::vec2 moveVec = glm::vec2(0);

        if (keyStates[SDL_SCANCODE_W] && keyStates[SDL_SCANCODE_S])
        {
            moveVec.x = 0;
        } else if (keyStates[SDL_SCANCODE_W])
        {
            moveVec.x = 1;
        } else if (keyStates[SDL_SCANCODE_S])
        {
            moveVec.x = -1;
        }

        if (keyStates[SDL_SCANCODE_D] && keyStates[SDL_SCANCODE_A])
        {
            moveVec.y = 0;
        } else if (keyStates[SDL_SCANCODE_A])
        {
            moveVec.y = 1;
        } else if (keyStates[SDL_SCANCODE_D])
        {
            moveVec.y = -1;
        }

        return moveVec;
    }

    glm::vec2 InputManager::GetMouseVector()
    {
        float deltaX = 0.0f;
        float deltaY = 0.0f;

        SDL_GetRelativeMouseState(&deltaX, &deltaY);

        return glm::vec2(deltaX, deltaY);
    }

    glm::vec2 InputManager::GetMousePosition()
    {
        float mouseX = 0.0f;
        float mouseY = 0.0f;

        SDL_GetMouseState(&mouseX, &mouseY);
        return glm::vec2(mouseX, Core::Screen::Height() - mouseY);
    }

    MouseInput InputManager::GetMouseInput()
    {
        bool isLeftDown = (currentMouseState & SDL_BUTTON_LMASK) != 0;
        bool isRightDown = (currentMouseState & SDL_BUTTON_RMASK) != 0;

        bool wasLeftDown = (previousMouseState & SDL_BUTTON_LMASK) != 0;
        bool wasRightDown = (previousMouseState & SDL_BUTTON_RMASK) != 0;

        return MouseInput{
            GetMousePosition(),
            isLeftDown && !wasLeftDown,
            isLeftDown,
            isRightDown && !wasRightDown,
            isRightDown};
    }

    ButtonInput InputManager::GetDebugButtonInput()
    {
        return debugKeyCache.UpdateInput();
    }
} // namespace Beer::System
