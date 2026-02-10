#pragma once

#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_raii.hpp>
#include "SDL_video.h"

namespace Beer::Core
{
    class SwapchainUtilities
    {
    public:
        static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> availableFormats);
        static bool IsCorrectFormat(const vk::SurfaceFormatKHR format);
        static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
        static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
            SDL_Window* window);
    };
} // namespace Beer::Core
