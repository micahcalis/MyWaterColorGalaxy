#pragma once

#include "SDL_video.h"
#include <SDL.h>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Core
{
    class SDLUtilities
    {
    public:
        static SDL_Window* CreateWindow(int width, int height);
        static bool SDLFailed();
        static bool PollEvents(bool& frameBufferResized);
        static std::vector<const char*> GetSDLExtensions(SDL_Window* window, const vk::raii::Context& context);
        static std::vector<const char*> GetRequiredExtensions(SDL_Window* window,
            const vk::raii::Context& context,
            const bool enableValidationLayers);
    };
} // namespace Beer::Core
