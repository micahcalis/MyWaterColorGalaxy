#pragma once

#define SDL_MAIN_HANDLED

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "Core/Application/Managers/WindowManager.hpp"
#include "Core/Application/Renderer/Renderer.hpp"

namespace Beer::Core
{
    class Application
    {
    private:
        WindowManager windowManager = {};
        Renderer renderer = {};
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
        void HandleWindowResize();
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
