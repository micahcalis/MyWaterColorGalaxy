#include "Core/Application/SDLUtilities.hpp"
#include <iostream>
#include "SDL_events.h"
#include <SDL2/SDL_vulkan.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Core
{
    SDL_Window* SDLUtilities::CreateWindow(int width, int height)
    {
        return SDL_CreateWindow("Vulkan",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    }

    bool SDLUtilities::SDLFailed()
    {
        bool failed = SDL_Init(SDL_INIT_VIDEO) != 0;

        if (failed)
        {
            std::cerr << "SDL Failed to Start: " << SDL_GetError() << std::endl;
        }

        return failed;
    }

    bool SDLUtilities::PollEvents(bool& frameBufferResized)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return false;
            } else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    frameBufferResized = true;
                }
            }
        }
        return true;
    }

    std::vector<const char*> SDLUtilities::GetSDLExtensions(SDL_Window* window, const vk::raii::Context& context)
    {
        unsigned int sdlExtensionCount = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr))
        {
            throw std::runtime_error("Failed to get SDL extension count");
        }

        std::vector<const char*> sdlExtensions(sdlExtensionCount);
        if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, sdlExtensions.data()))
        {
            throw std::runtime_error("Failed to get SDL extensions");
        }

        auto extensionProperties = context.enumerateInstanceExtensionProperties();

        for (const char* requiredExt : sdlExtensions)
        {
            if (std::ranges::none_of(extensionProperties,
                    [requiredExt](auto const& property) {
                        return strcmp(property.extensionName, requiredExt) == 0;
                    }))
            {
                throw std::runtime_error("Required SDL extension not supported: " + std::string(requiredExt));
            }
        }

        return sdlExtensions;
    }

    std::vector<const char*> SDLUtilities::GetRequiredExtensions(SDL_Window* window,
        const vk::raii::Context& context,
        const bool enableValidationLayers)
    {
        std::vector<const char*> extensions = SDLUtilities::GetSDLExtensions(window, context);

        if (enableValidationLayers)
        {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
        }

        return extensions;
    }
} // namespace Beer::Core
