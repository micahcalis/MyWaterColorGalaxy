#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Core
{
    class SDLUtilities
    {
    public:
        static SDL_Window* CreateWindow(int width, int height);
        static bool SDLInitialize();
        static bool PollEvents(bool& frameBufferResized);
        static std::vector<const char*> GetSDLExtensions(const vk::raii::Context& context);
        static std::vector<const char*> GetRequiredExtensions(const vk::raii::Context& context,
            const bool enableValidationLayers);
    };
} // namespace Beer::Core
