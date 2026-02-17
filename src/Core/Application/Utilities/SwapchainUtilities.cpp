#include "Core/Application/Utilities/SwapchainUtilities.hpp"

namespace Beer::Core
{
    vk::SurfaceFormatKHR SwapchainUtilities::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> availableFormats)
    {
        assert(!availableFormats.empty());
        const auto formatIt = std::ranges::find_if(
            availableFormats,
            [](const auto& format) { return IsCorrectFormat(format); });

        return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
    }

    bool SwapchainUtilities::IsCorrectFormat(const vk::SurfaceFormatKHR format)
    {
        return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    }

    vk::PresentModeKHR SwapchainUtilities::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
    {
        for (const auto presentMode : availablePresentModes)
        {
            if (presentMode == vk::PresentModeKHR::eMailbox)
            {
                return presentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D SwapchainUtilities::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
        SDL_Window* window)
    {
        int width = 0;
        int height = 0;
        SDL_GetWindowSizeInPixels(window, &width, &height);

        vk::Extent2D extent{};
        extent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return extent;
    }
} // namespace Beer::Core
