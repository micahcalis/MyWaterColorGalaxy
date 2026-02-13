#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Beer::Core
{
    class WindowManager
    {
    private:
        SDL_Window* window;

    public:
        void InitializeWindow();
        void Cleanup();
        SDL_Window* GetWindow() const;
    };
} // namespace Beer::Core
