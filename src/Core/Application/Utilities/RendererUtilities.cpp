#pragma once

#include "Core/Application/Utilities/RendererUtilities.hpp"
#include "RendererUtilities.hpp"
#include "vulkan/vulkan.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>
#include <cstring>

namespace Beer::Core
{
    bool RendererUtilities::AcquireNextImage(Swapchain& swapchain,
        const FrameResource& frameResource,
        uint32_t& imageIndex)
    {
        auto [result, index] = swapchain.AcquireNextImage(*frameResource.GetImageAvailableSemaphore());
        imageIndex = index;
        if (result == vk::Result::eSuccess)
            return true;

        assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady && "Swapchain result timeout or not ready");

        std::println(std::cerr, "failed to acquire swap chain image!");

        return false;
    }

    vk::RenderingAttachmentInfo RendererUtilities::CreateColorAttachmentInfo(vk::ImageView imageView, vk::ClearValue clearColor)
    {
        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.imageView = imageView;
        colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.clearValue = clearColor;

        return colorAttachment;
    }

    vk::RenderingInfo RendererUtilities::CreateRenderingInfo(vk::Extent2D extent, const vk::RenderingAttachmentInfo& colorAttachment)
    {
        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea = vk::Rect2D({0, 0}, extent);
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        return renderingInfo;
    }

    vk::SubmitInfo RendererUtilities::CreateSubmitInfo(const FrameResource& frameResource,
        const vk::CommandBuffer& commandBuffer,
        vk::PipelineStageFlags* waitMask)
    {
        vk::SubmitInfo submitInfo{};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &*frameResource.GetImageAvailableSemaphore();
        submitInfo.pWaitDstStageMask = waitMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &*frameResource.GetRenderFinishedSemaphore();

        return submitInfo;
    }

    vk::PresentInfoKHR RendererUtilities::CreatePresentInfo(const FrameResource& frameResource,
        const Swapchain& swapchain,
        const uint32_t& imageIndex)

    {
        auto& raiiSwapchain = swapchain.GetRaiiSwapchain();

        vk::PresentInfoKHR presentInfoKHR{};
        presentInfoKHR.waitSemaphoreCount = 1;
        presentInfoKHR.pWaitSemaphores = &*frameResource.GetRenderFinishedSemaphore();
        presentInfoKHR.swapchainCount = 1;
        presentInfoKHR.pSwapchains = &*raiiSwapchain;
        presentInfoKHR.pImageIndices = &imageIndex;
        presentInfoKHR.pResults = nullptr;

        return presentInfoKHR;
    }

    /**
     * @brief vk::raii::Queue::presentKHR without exceptions
     * Source: https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
     */
    vk::Result RendererUtilities::Queue_PresentKHR_NoExcept(const vk::raii::Queue& queue,
        const vk::PresentInfoKHR& present_info)
    {
        return static_cast<vk::Result>(queue.getDispatcher()->vkQueuePresentKHR(
            static_cast<VkQueue>(*queue),
            reinterpret_cast<const VkPresentInfoKHR*>(&present_info)));
    }

    void RendererUtilities::CreateBuffer(vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        const Device& device,
        vk::raii::Buffer& buffer,
        vk::raii::DeviceMemory& bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.size = size;
        bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        buffer = vk::raii::Buffer(device.GetLogicalDevice(), bufferInfo);

        vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

        vk::MemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.allocationSize = memRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = RendererUtilities::FindMemoryType(memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            device.GetPhysicalDevice());

        bufferMemory = vk::raii::DeviceMemory(device.GetLogicalDevice(), memoryAllocateInfo);
        buffer.bindMemory(bufferMemory, 0);
    }

    uint32_t RendererUtilities::FindMemoryType(uint32_t typeFilter,
        vk::MemoryPropertyFlags properties,
        const vk::raii::PhysicalDevice& physicalDevice)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            // 1. Check if the memory type bit is set in the filter
            bool typeIsSuitable = (typeFilter & (1 << i));

            // 2. CHECK THE PROPERTIES! (This was missing)
            // We ensure the memory type supports ALL the flags we asked for.
            bool propertiesAreSuitable = (memProperties.memoryTypes[i].propertyFlags & properties) == properties;

            if (typeIsSuitable && propertiesAreSuitable)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void RendererUtilities::MapVertices(vk::raii::Buffer& vertexBuffer,
        vk::raii::DeviceMemory& vertexBufferMemory,
        const std::vector<Rendering::Vertex>& vertices,
        const size_t size)
    {
        void* data = vertexBufferMemory.mapMemory(0, size);
        memcpy(data, vertices.data(), size);
        vertexBufferMemory.unmapMemory();
    }

    void RendererUtilities::CopyBuffer(vk::raii::Buffer& srcBuffer,
        vk::raii::Buffer& dstBuffer,
        vk::DeviceSize size,
        const Device& device,
        const FrameResource& frameResource)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = frameResource.GetCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        vk::raii::CommandBuffer commandCopyBuffer = std::move(device.GetLogicalDevice().allocateCommandBuffers(allocInfo).front());
        commandCopyBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
        commandCopyBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*commandCopyBuffer;

        device.GetGraphicsQueue().submit(submitInfo, nullptr);
        device.GetGraphicsQueue().waitIdle();
    }
} // namespace Beer::Core
