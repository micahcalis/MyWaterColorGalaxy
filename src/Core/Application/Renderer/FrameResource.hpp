#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Renderer/Device.hpp"

namespace Beer::Core
{
    class FrameResource
    {
    private:
        const Device* device = nullptr;
        vk::raii::CommandPool commandPool = nullptr;
        vk::raii::CommandBuffer commandBuffer = nullptr;
        vk::raii::Semaphore imageAvailableSemaphore = nullptr;
        vk::raii::Semaphore renderFinishedSemaphore = nullptr;
        vk::raii::Fence inFlightFence = nullptr;

    public:
        FrameResource(const Device* device);
        void Reset();
        const vk::raii::Fence& GetInFlightFence() const;
        const vk::raii::Semaphore& GetImageAvailableSemaphore() const;
        const vk::raii::Semaphore& GetRenderFinishedSemaphore() const;
        vk::CommandBuffer GetCommandBuffer();
        void CreateSyncObjects();

    private:
        void CreateCommandPool();
        void CreateCommandBuffer();
    };
} // namespace Beer::Core
