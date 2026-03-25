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
        [[nodiscard]] const vk::raii::SwapchainKHR& GetRaiiSwapchain() const;
        const vk::SurfaceFormatKHR& GetSurfaceFormat();
        [[nodiscard]] const vk::Format& GetImageFormat() const;
        [[nodiscard]] vk::Extent2D GetExtent() const;
        vk::ResultValue<uint32_t> AcquireNextImage(vk::Semaphore semaphore);
        [[nodiscard]] const vk::Image& GetImage(uint32_t imageIndex) const;
        [[nodiscard]] uint32_t GetSwapchainCount() const;
        [[nodiscard]] const vk::raii::ImageView& GetImageView(uint32_t imageIndex) const;

    private:
        void CreateSwapchain(SDL_Window* window,
            const vk::raii::SurfaceKHR& surface,
            const Device& device);

        void CreateImageViews(const Device& device);

    private:
        static inline VkFormat chainFormat = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
        static void SetFormat(VkFormat format) { chainFormat = format; }

    public:
        static VkFormat GetFormat() { return chainFormat; }
    };
} // namespace Beer::Core
