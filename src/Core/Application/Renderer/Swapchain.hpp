#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Renderer/Device.hpp"
#include "SDL3/SDL_video.h"
#include <vector>

namespace Beer::Core
{
    class Swapchain
    {
    private:
        vk::raii::SwapchainKHR swapchain = nullptr;
        vk::SurfaceFormatKHR swapchainSurfaceFormat;
        vk::Format swapchainImageFormat;
        vk::Extent2D swapchainExtent;
        std::vector<vk::Image> swapchainImages;
        std::vector<vk::raii::ImageView> swapChainImageViews;

    public:
        void InitializeSwapchain(SDL_Window* window,
            const vk::raii::SurfaceKHR& surface,
            const Device& device);

        void RecreateSwapchain(SDL_Window* window,
            const vk::raii::SurfaceKHR& surface,
            const Device& device);

        void CleanupSwapchain();
        const vk::raii::SwapchainKHR& GetRaiiSwapchain() const;
        const vk::SurfaceFormatKHR& GetSurfaceFormat();
        const vk::Format& GetImageFormat() const;
        const vk::Extent2D GetExtent() const;
        vk::ResultValue<uint32_t> AcquireNextImage(vk::Semaphore semaphore);
        const vk::Image& GetImage(uint32_t imageIndex) const;
        const uint32_t GetSwapchainCount() const;
        const vk::raii::ImageView& GetImageView(uint32_t imageIndex) const;

    private:
        void CreateSwapchain(SDL_Window* window,
            const vk::raii::SurfaceKHR& surface,
            const Device& device);

        void CreateImageViews(const Device& device);
    };
} // namespace Beer::Core
