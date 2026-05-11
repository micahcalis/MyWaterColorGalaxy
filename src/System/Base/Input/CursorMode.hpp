#pragma once

#include "Core/Application/Managers/WindowManager.hpp"
#include <stdexcept>
namespace Beer::Core
{
    class WindowManager;
}

namespace Beer::System
{
    enum class CursorMode
    {
        Unlocked,
        Locked
    };

    class Cursor
    {
    private:
        inline static CursorMode currentMode = CursorMode::Unlocked;
        inline static Core::WindowManager* windowManager = nullptr;

    public:
        static void SetCursorMode(const CursorMode mode);
        static CursorMode GetCurrentMode()
        {
            if (windowManager == nullptr)
            {
                throw std::runtime_error("Cursor Has Null Ref to WindowManager");
            }

            return currentMode;
        }

        static void SetWindowManager(Core::WindowManager* windowManager)
        {
            Cursor::windowManager = windowManager;
        }
    };
} // namespace Beer::System
