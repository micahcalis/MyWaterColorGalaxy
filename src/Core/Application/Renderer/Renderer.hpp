#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL_video.h>
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Renderer/PipelineCache.hpp"
#include <memory>
#include <vector>
#include "Core/Application/Renderer/FrameResource.hpp"

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
        Swapchain swapchain{};
        std::unique_ptr<PipelineCache> pipelineCache = nullptr;
        std::vector<FrameResource> frameResources;

    public:
        void InitializeVulkanInstances(SDL_Window* window);
        void RecreateSwapchain(SDL_Window* window);
        const vk::raii::Context& GetContext() const;
        const vk::raii::Instance& GetInstance() const;
        const vk::raii::DebugUtilsMessengerEXT& GetDebugMessenger() const;
        const vk::raii::SurfaceKHR& GetSurface() const;
        const Device& GetDevice() const;
        Swapchain& GetSwapchain();

    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface(SDL_Window* window);
    };
} // namespace Beer::Core
