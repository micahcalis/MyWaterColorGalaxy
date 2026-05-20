#pragma once

#include "System/Base/Input./CursorMode.hpp"
#include "Core/Application/Managers/WindowManager.hpp"

namespace Beer::System
{
    void Cursor::SetCursorMode(const CursorMode mode)
    {
        if (windowManager == nullptr)
        {
            throw std::runtime_error("Cursor Has Null Ref to WindowManager");
        }

        currentMode = mode;
        SDL_SetWindowRelativeMouseMode(windowManager->GetWindow(), mode == CursorMode::Locked);
    }
} // namespace Beer::System
