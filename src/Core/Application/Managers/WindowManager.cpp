#include "Core/Application/Managers/WindowManager.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"

namespace Beer::Core
{
    constexpr uint32_t APP_WIDTH = 800;
    constexpr uint32_t APP_HEIGHT = 600;

    void WindowManager::InitializeWindow()
    {
        if (!SDLUtilities::SDLInitialize())
        {
            throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
        }

        window = SDLUtilities::CreateWindow(APP_WIDTH, APP_HEIGHT);

        if (window == nullptr)
        {
            throw std::runtime_error("Window Creation Failed: " + std::string(SDL_GetError()));
        }
    }

    void WindowManager::Cleanup()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_Window* WindowManager::GetWindow()
    {
        return window;
    }
} // namespace Beer::Core
