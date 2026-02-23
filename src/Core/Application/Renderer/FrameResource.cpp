#include "vulkan/vulkan.hpp"
#include <Core/Application/Renderer/FrameResource.hpp>

namespace Beer::Core
{
    FrameResource::FrameResource(const Device* device)
        : device(device)
    {
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
    }

    void FrameResource::Reset()
    {
        device->GetLogicalDevice().resetFences({*inFlightFence});
        commandPool.reset();
    }

    const vk::raii::CommandPool& FrameResource::GetCommandPool() const { return commandPool; }
    const vk::raii::Fence& FrameResource::GetInFlightFence() const { return inFlightFence; }
    const vk::raii::Semaphore& FrameResource::GetImageAvailableSemaphore() const { return imageAvailableSemaphore; }
    const vk::raii::Semaphore& FrameResource::GetRenderFinishedSemaphore() const { return renderFinishedSemaphore; }
    vk::CommandBuffer FrameResource::GetCommandBuffer() { return commandBuffer; }

    void FrameResource::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex = device->GetGraphicsIndex();

        commandPool = vk::raii::CommandPool(device->GetLogicalDevice(), commandPoolCreateInfo);
    }

    void FrameResource::CreateCommandBuffer()
    {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        commandBufferAllocateInfo.commandBufferCount = 1;

        vk::raii::CommandBuffers buffers(device->GetLogicalDevice(), commandBufferAllocateInfo);
        commandBuffer = std::move(buffers[0]);
    }

    void FrameResource::CreateSyncObjects()
    {
        renderFinishedSemaphore = vk::raii::Semaphore(device->GetLogicalDevice(), vk::SemaphoreCreateInfo());
        imageAvailableSemaphore = vk::raii::Semaphore(device->GetLogicalDevice(), vk::SemaphoreCreateInfo());
        inFlightFence = vk::raii::Fence(device->GetLogicalDevice(), {vk::FenceCreateFlagBits::eSignaled});
    }
} // namespace Beer::Core
