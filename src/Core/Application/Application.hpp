#pragma once

#define SDL_MAIN_HANDLED

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <Core/Application/Managers/Managers.hpp>

namespace Beer::Core
{
    class Application
    {
    private:
        WindowManager windowManager = {};
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
        vk::raii::SurfaceKHR surface = nullptr;
        vk::raii::PhysicalDevice physicalDevice = nullptr;
        std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};
        vk::raii::Device device = nullptr;
        vk::PhysicalDeviceFeatures deviceFeatures;
        vk::raii::Queue graphicsQueue = nullptr;
        vk::raii::Queue presentQueue = nullptr;
        uint32_t graphicsIndex;
        vk::SurfaceFormatKHR swaphchainSurfaceFormat;
        vk::Format swapchainImageFormat;
        vk::Extent2D swapchainExtent;
        vk::raii::SwapchainKHR swapchain = nullptr;
        std::vector<vk::Image> swapchainImages;
        std::vector<vk::raii::ImageView> swapChainImageViews;
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::Pipeline graphicsPipeline = nullptr;
        vk::raii::CommandPool commandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> commandBuffers;
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
        std::vector<vk::raii::Fence> inFlightFences;
        uint32_t frameIndex = 0;
        bool frameBufferResized = false;

    public:
        void Run();

    private:
        void InitializeVulkan();
        void MainLoop();
        void Cleanup();
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapchain();
        void CleanupSwapchain();
        void RecreateSwapchain();
        void CreateImageViews();
        void CreateGraphicsPipeline();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateSyncObjects();
        void RecordCommandBuffer(vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex);
        void DrawFrame();

        void TransitionImageLayout(vk::CommandBuffer commandBuffer,
            uint32_t imageIndex,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            vk::AccessFlags2 srcAccessMask,
            vk::AccessFlags2 dstAccessMask,
            vk::PipelineStageFlags2 srcStageMask,
            vk::PipelineStageFlags2 dstStageMask);
    };
} // namespace Beer::Core
