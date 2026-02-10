#pragma once

#define SDL_MAIN_HANDLED

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>
#include <SDL.h>
#include <filesystem>

namespace Beer::Core
{
    class Application
    {
    private:
        SDL_Window* window = nullptr;
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
        void InitializeWindow();
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
        std::vector<char const*> GetRequiredLayers();
        bool ValidationLayersUnsupported(std::vector<char const*> requiredLayers, std::vector<vk::LayerProperties> layerProperties);
        std::vector<const char*> GetRequiredExtensions();
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
        void FilterPhysicalDevices(std::vector<vk::raii::PhysicalDevice>& devices);
        static uint32_t GetPhysicalDeviceScore(const vk::raii::PhysicalDevice& device, bool& hasGeomShader);
        void GetQueueFamilyIndices(const std::vector<vk::QueueFamilyProperties> queueFamilyProperties, uint32_t& graphicsIndex, uint32_t& presentIndex);
        bool IndexIsCompatible(const int presentIndex, const int queueFamilyLength);
        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> availableFormats);
        static bool IsCorrectFormat(const vk::SurfaceFormatKHR format);
        vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
        [[nodiscard]] static std::vector<char> ReadFile(const std::filesystem::path& path);
        std::string GetAssetPath(const std::string& subPath);
        [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code);

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
