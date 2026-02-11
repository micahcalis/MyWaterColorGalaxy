#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL_video.h>
#include "Core/Application/Renderer/Device.hpp"

namespace Beer::Core
{
    class Renderer
    {
    private:
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
        vk::raii::SurfaceKHR surface = nullptr;
        Device device{};

    public:
        void InitializeVulkanInstances(SDL_Window* window);
        const vk::raii::Context& GetContext() const;
        const vk::raii::Instance& GetInstance() const;
        const vk::raii::DebugUtilsMessengerEXT& GetDebugMessenger() const;
        const vk::raii::SurfaceKHR& GetSurface() const;
        const Device& GetDevice() const;
        vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities() const;
        std::vector<vk::SurfaceFormatKHR> GetAvailableFormats() const;
        const std::vector<vk::PresentModeKHR> GetAvailablePresentModes() const;

    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface(SDL_Window* window);
    };
} // namespace Beer::Core
