#include "Core/Application/Renderer/Swapchain.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vector>
#include "Core/Application/Utilities/SwapchainUtilities.hpp"
#include "SDL3/SDL_events.h"

namespace Beer::Core
{
    void Swapchain::InitializeSwapchain(SDL_Window* window,
        const vk::raii::SurfaceKHR& surface,
        const Device& device)
    {
        CreateSwapchain(window, surface, device);
        CreateImageViews(device);
    }

    const vk::raii::SwapchainKHR& Swapchain::GetRaiiSwapchain() const { return swapchain; }
    const vk::SurfaceFormatKHR& Swapchain::GetSurfaceFormat() { return swapchainSurfaceFormat; }
    const vk::Format& Swapchain::GetImageFormat() const { return swapchainImageFormat; }
    vk::Extent2D Swapchain::GetExtent() const { return swapchainExtent; }
    const vk::Image& Swapchain::GetImage(uint32_t imageIndex) const { return swapchainImages[imageIndex]; }
    uint32_t Swapchain::GetSwapchainCount() const { return swapchainImages.size(); }
    const vk::raii::ImageView& Swapchain::GetImageView(uint32_t imageIndex) const { return swapChainImageViews[imageIndex]; }

    void Swapchain::CreateSwapchain(SDL_Window* window,
        const vk::raii::SurfaceKHR& surface,
        const Device& device)
    {
        vk::SurfaceCapabilitiesKHR surfaceCapabilities = device.GetSurfaceCapabilities(surface);
        std::vector<vk::SurfaceFormatKHR> availableFormats = device.GetAvailableFormats(surface);

        swapchainSurfaceFormat = SwapchainUtilities::ChooseSwapSurfaceFormat(availableFormats);
        swapchainExtent = SwapchainUtilities::ChooseSwapExtent(surfaceCapabilities, window);
        auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
        minImageCount = (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount) ? surfaceCapabilities.maxImageCount : minImageCount;

        uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = minImageCount;
        swapchainCreateInfo.imageFormat = swapchainSurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = swapchainSurfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = swapchainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchainCreateInfo.presentMode = SwapchainUtilities::ChooseSwapPresentMode(device.GetAvailablePresentModes(surface));
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = nullptr;

        swapchain = vk::raii::SwapchainKHR(device.GetLogicalDevice(), swapchainCreateInfo);
        swapchainImages = swapchain.getImages();
        swapchainImageFormat = swapchainSurfaceFormat.format;
    }

    void Swapchain::CreateImageViews(const Device& device)
    {
        swapChainImageViews.clear();

        vk::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = swapchainImageFormat;
        imageViewCreateInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

        for (vk::Image image : swapchainImages)
        {
            imageViewCreateInfo.image = image;
            swapChainImageViews.emplace_back(device.GetLogicalDevice(), imageViewCreateInfo);
        }
    }

    void Swapchain::RecreateSwapchain(SDL_Window* window,
        const vk::raii::SurfaceKHR& surface,
        const Device& device)
    {
        int width = 0;
        int height = 0;
        SDL_GetWindowSizeInPixels(window, &width, &height);

        while (width == 0 || height == 0)
        {
            SDL_GetWindowSizeInPixels(window, &width, &height);
            SDL_WaitEvent(nullptr);
        }

        device.GetLogicalDevice().waitIdle();
        CleanupSwapchain();
        CreateSwapchain(window, surface, device);
        CreateImageViews(device);
    }

    void Swapchain::CleanupSwapchain()
    {
        swapChainImageViews.clear();
        swapchain = nullptr;
    }

    vk::ResultValue<uint32_t> Swapchain::AcquireNextImage(vk::Semaphore semaphore)
    {
        return swapchain.acquireNextImage(UINT64_MAX, semaphore, nullptr);
    }
} // namespace Beer::Core
