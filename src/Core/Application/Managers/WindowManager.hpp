#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Beer::Core
{
    static const uint32_t APP_WINDOWED_WIDTH = 800;
    static const uint32_t APP_WINDOWED_HEIGHT = 600;

    class WindowManager
    {
    private:
        SDL_Window* window;
        int windowedWidth = APP_WINDOWED_WIDTH;
        int windowedHeight = APP_WINDOWED_HEIGHT;
        bool isFullscreen = true;

    public:
        void InitializeWindow();
        void Cleanup();
        void UpdateWindowedResolution();
        void ToggleFullscreen();
        void QuitApplication();
        SDL_Window* GetWindow() const;
        bool GetFullscreen() const { return isFullscreen; };
    };
} // namespace Beer::Core
