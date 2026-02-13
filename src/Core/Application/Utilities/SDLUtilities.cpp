#include "Core/Application/Utilities/SDLUtilities.hpp"
#include <SDL3/SDL_video.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace Beer::Core
{
    SDL_Window* SDLUtilities::CreateWindow(int width, int height)
    {
        return SDL_CreateWindow("Vulkan",
            width,
            height,
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    }

    bool SDLUtilities::SDLInitialize()
    {
        bool success = SDL_Init(SDL_INIT_VIDEO) != 0;

        if (!success)
        {
            std::cerr << "SDL Failed to Start: " << SDL_GetError() << std::endl;
        }

        return success;
    }

    bool SDLUtilities::PollEvents(bool& frameBufferResized)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                return false;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                frameBufferResized = true;
            }
        }
        return true;
    }

    std::vector<const char*> SDLUtilities::GetSDLExtensions(const vk::raii::Context& context)
    {
        unsigned int sdlExtensionCount = 0;
        const char* const* sdlExtensionsRaw = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

        if (sdlExtensionsRaw == nullptr)
        {
            throw std::runtime_error("Failed to get SDL extensions: " + std::string(SDL_GetError()));
        }

        std::vector<const char*> sdlExtensions(sdlExtensionsRaw, sdlExtensionsRaw + sdlExtensionCount);

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

    std::vector<const char*> SDLUtilities::GetRequiredExtensions(const vk::raii::Context& context,
        const bool enableValidationLayers)
    {
        std::vector<const char*> extensions = SDLUtilities::GetSDLExtensions(context);

        if (enableValidationLayers)
        {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
        }

        return extensions;
    }
} // namespace Beer::Core
