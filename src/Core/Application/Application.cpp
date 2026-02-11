#include "Core/Application/Application.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "vulkan/vulkan.hpp"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <vector>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

namespace Beer::Core
{
    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    void Application::Run()
    {
        windowManager.InitializeWindow();
        InitializeVulkan();
        MainLoop();
        Cleanup();
    }

    void Application::InitializeVulkan()
    {
        renderer.InitializeVulkanInstances(windowManager.GetWindow());
        CreateGraphicsPipeline();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    void Application::MainLoop()
    {
        while (SDLUtilities::PollEvents(frameBufferResized))
        {
            DrawFrame();
        }

        renderer.GetDevice().GetLogicalDevice().waitIdle();
    }

    void Application::Cleanup()
    {
        windowManager.Cleanup();
        renderer.GetSwapchain().CleanupSwapchain();
    }

    void Application::HandleWindowResize()
    {
        renderer.RecreateSwapchain(windowManager.GetWindow());
        CreateSyncObjects();
    }

    void Application::CreateGraphicsPipeline()
    {
        auto shaderCode = AssetUtilities::ReadFile(AssetUtilities::GetAssetPath("assets/shaders/HelloTriangle.spv"));
        auto shaderModule = AssetUtilities::CreateShaderModule(shaderCode, renderer.GetDevice().GetLogicalDevice());

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertShaderStageInfo.module = shaderModule;
        vertShaderStageInfo.pName = "HelloTriangleVert";

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragShaderStageInfo.module = shaderModule;
        fragShaderStageInfo.pName = "HelloTriangleFrag";

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

        std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

        vk::PipelineDynamicStateCreateInfo dynamicCreateInfo{};
        dynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicCreateInfo.pDynamicStates = dynamicStates.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
        inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

        const vk::Extent2D swapchainExtent = renderer.GetSwapchain().GetExtent();
        vk::Viewport viewPort = vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f);
        vk::Rect2D region = vk::Rect2D(vk::Offset2D{0, 0}, swapchainExtent);

        vk::PipelineViewportStateCreateInfo viewportCreateInfo{};
        viewportCreateInfo.viewportCount = 1;
        viewportCreateInfo.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
        rasterizationCreateInfo.depthClampEnable = vk::False;
        rasterizationCreateInfo.rasterizerDiscardEnable = vk::False;
        rasterizationCreateInfo.polygonMode = vk::PolygonMode::eFill;
        rasterizationCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
        rasterizationCreateInfo.frontFace = vk::FrontFace::eClockwise;
        rasterizationCreateInfo.depthBiasEnable = vk::False;
        rasterizationCreateInfo.depthBiasSlopeFactor = 1.0f;
        rasterizationCreateInfo.lineWidth = 1.0f;

        vk::PipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
        multisamplingCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisamplingCreateInfo.sampleShadingEnable = vk::False;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = vk::True;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;

        vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
        colorBlendCreateInfo.logicOpEnable = vk::False;
        colorBlendCreateInfo.logicOp = vk::LogicOp::eCopy;
        colorBlendCreateInfo.attachmentCount = 1;
        colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pushConstantRangeCount = 0;

        pipelineLayout = vk::raii::PipelineLayout(renderer.GetDevice().GetLogicalDevice(), layoutCreateInfo);

        vk::PipelineRenderingCreateInfo renderingCreateInfo{};
        vk::Format colorFormat = renderer.GetSwapchain().GetImageFormat();
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &colorFormat;

        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.pNext = &renderingCreateInfo;
        graphicsPipelineCreateInfo.stageCount = 2;
        graphicsPipelineCreateInfo.pStages = shaderStages;
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicCreateInfo;
        graphicsPipelineCreateInfo.layout = pipelineLayout;
        graphicsPipelineCreateInfo.renderPass = nullptr;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        graphicsPipeline = vk::raii::Pipeline(renderer.GetDevice().GetLogicalDevice(), nullptr, graphicsPipelineCreateInfo);
    }

    void Application::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex = renderer.GetDevice().GetGraphicsIndex();

        commandPool = vk::raii::CommandPool(renderer.GetDevice().GetLogicalDevice(), commandPoolCreateInfo);
    }

    void Application::CreateCommandBuffers()
    {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        commandBufferAllocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

        commandBuffers = vk::raii::CommandBuffers(renderer.GetDevice().GetLogicalDevice(), commandBufferAllocateInfo);
    }

    void Application::CreateSyncObjects()
    {
        presentCompleteSemaphores.clear();
        renderFinishedSemaphores.clear();
        inFlightFences.clear();

        // assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

        for (size_t i = 0; i < renderer.GetSwapchain().GetSwapchainCount(); i++)
        {
            renderFinishedSemaphores.emplace_back(renderer.GetDevice().GetLogicalDevice(), vk::SemaphoreCreateInfo());
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            presentCompleteSemaphores.emplace_back(renderer.GetDevice().GetLogicalDevice(), vk::SemaphoreCreateInfo());
            inFlightFences.emplace_back(vk::raii::Fence(renderer.GetDevice().GetLogicalDevice(), {vk::FenceCreateFlagBits::eSignaled}));
        }
    }

    void Application::RecordCommandBuffer(vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        commandBuffer.begin({});

        TransitionImageLayout(commandBuffers[frameIndex],
            imageIndex,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput);

        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f);

        vk::RenderingAttachmentInfo renderingAttachmentInfo{};
        renderingAttachmentInfo.imageView = renderer.GetSwapchain().GetImageView(imageIndex);
        renderingAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        renderingAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        renderingAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        renderingAttachmentInfo.clearValue = clearColor;

        vk::RenderingInfo renderingInfo{};
        vk::Extent2D swapchainExtent = renderer.GetSwapchain().GetExtent();
        renderingInfo.renderArea = vk::Rect2D({0, 0}, swapchainExtent);
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &renderingAttachmentInfo;

        commandBuffer.beginRendering(renderingInfo);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

        commandBuffer.setViewport(0,
            vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f));

        commandBuffer.setScissor(0,
            vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));

        commandBuffer.draw(3, 1, 0, 0);

        commandBuffer.endRendering();

        TransitionImageLayout(commandBuffers[frameIndex],
            imageIndex,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe);

        commandBuffer.end();
    }

    void Application::DrawFrame()
    {
        auto fenceResult = renderer.GetDevice().GetLogicalDevice().waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);

        if (fenceResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to wait for fence");
        }

        uint32_t imageIndex = 0;
        vk::Result result;

        try
        {
            auto acquireResult = renderer.GetSwapchain().AcquireNextImage(*presentCompleteSemaphores[frameIndex]);
            result = acquireResult.result;
            imageIndex = acquireResult.value;
        } catch (const vk::OutOfDateKHRError& e)
        {
            HandleWindowResize();
            return;
        }

        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        renderer.GetDevice().GetLogicalDevice().resetFences(*inFlightFences[frameIndex]);
        RecordCommandBuffer(commandBuffers[frameIndex], imageIndex);

        auto waitDestinationStageMask = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);

        vk::SubmitInfo submitInfo{};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &*presentCompleteSemaphores[frameIndex];
        submitInfo.pWaitDstStageMask = &waitDestinationStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*commandBuffers[frameIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &*renderFinishedSemaphores[imageIndex];

        renderer.GetDevice().GetGraphicsQueue().submit(submitInfo, *inFlightFences[frameIndex]);
        auto& raiiSwapchain = renderer.GetSwapchain().GetRaiiSwapchain();

        vk::PresentInfoKHR presentInfoKHR{};
        presentInfoKHR.waitSemaphoreCount = 1;
        presentInfoKHR.pWaitSemaphores = &*renderFinishedSemaphores[imageIndex];
        presentInfoKHR.swapchainCount = 1;
        presentInfoKHR.pSwapchains = &*raiiSwapchain;
        presentInfoKHR.pImageIndices = &imageIndex;
        presentInfoKHR.pResults = nullptr;

        try
        {
            result = renderer.GetDevice().GetPresentQueue().presentKHR(presentInfoKHR);
        } catch (const vk::OutOfDateKHRError& e)
        {
            HandleWindowResize();
        }

        if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) || frameBufferResized)
        {
            frameBufferResized = false;
            HandleWindowResize();
        } else
        {
            assert(result == vk::Result::eSuccess);
        }

        frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Application::TransitionImageLayout(vk::CommandBuffer commandBuffer,
        uint32_t imageIndex,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask,
        vk::AccessFlags2 dstAccessMask,
        vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask)
    {
        vk::ImageMemoryBarrier2 barrier{};
        barrier.srcStageMask = srcStageMask;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstStageMask = dstStageMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = renderer.GetSwapchain().GetImage(imageIndex);
        barrier.subresourceRange = {
            vk::ImageAspectFlagBits::eColor,
            0,
            1,
            0,
            1};

        vk::DependencyInfo dependencyInfo{};
        dependencyInfo.dependencyFlags = {};
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &barrier;

        commandBuffer.pipelineBarrier2(dependencyInfo);
    }
} // namespace Beer::Core
