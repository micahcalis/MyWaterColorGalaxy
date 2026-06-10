#include "Core/Application/Managers/WindowManager.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"
#include "SDL3/SDL_video.h"
#include <print>

namespace Beer::Core
{
    void WindowManager::InitializeWindow()
    {
        if (!SDLUtilities::SDLInitialize())
        {
            throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
        }

        window = SDLUtilities::CreateWindow(APP_WINDOWED_WIDTH, APP_WINDOWED_HEIGHT);

        if (window == nullptr)
        {
            throw std::runtime_error("Window Creation Failed: " + std::string(SDL_GetError()));
        }
    }

    void WindowManager::UpdateWindowedResolution()
    {
        SDL_GetWindowSizeInPixels(window, &windowedWidth, &windowedHeight);
    }

    void WindowManager::ToggleFullscreen()
    {
        isFullscreen = !isFullscreen;
        SDL_SetWindowFullscreen(window, isFullscreen);

        if (!isFullscreen)
        {
            std::println("set thibng");
            SDL_SetWindowSize(window, windowedWidth, windowedHeight);
        }
    }

    void WindowManager::Cleanup()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_Window* WindowManager::GetWindow() const
    {
        return window;
    }

} // namespace Beer::Core
