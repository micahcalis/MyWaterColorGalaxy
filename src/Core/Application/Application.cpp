#include "Core/Application/Application.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Application/Utilities/VulkanInitUtilities.hpp"
#include "Core/Application/Utilities/SwapchainUtilities.hpp"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

namespace Beer::Core
{
    constexpr uint32_t APP_WIDTH = 800;
    constexpr uint32_t APP_HEIGHT = 600;
    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<char const*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
    constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

    void Application::Run()
    {
        InitializeWindow();
        InitializeVulkan();
        MainLoop();
        Cleanup();
    }

    void Application::InitializeWindow()
    {
        if (!SDLUtilities::SDLInitialize())
        {
            throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
        }

        window = SDLUtilities::CreateWindow(APP_WIDTH, APP_HEIGHT);

        if (window == nullptr)
        {
            throw std::runtime_error("Window Creation Failed: " + std::string(SDL_GetError()));
        }
    }

    void Application::InitializeVulkan()
    {
        CreateInstance();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapchain();
        CreateImageViews();
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

        device.waitIdle();
    }

    void Application::Cleanup()
    {
        CleanupSwapchain();
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Application::CreateInstance()
    {
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = vk::ApiVersion14;

        std::vector<const char*> sdlExtensions = SDLUtilities::GetRequiredExtensions(context,
            ENABLE_VALIDATION_LAYERS);

        std::vector<const char*> requiredLayers = VulkanInitUtilities::GetRequiredLayers(ENABLE_VALIDATION_LAYERS,
            validationLayers,
            context);

        vk::InstanceCreateInfo createInfo({}, &appInfo, requiredLayers, sdlExtensions);

        instance = vk::raii::Instance(context, createInfo);
    }

    void Application::SetupDebugMessenger()
    {
        if (!ENABLE_VALIDATION_LAYERS)
            return;

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        debugMessengerCreateInfo.messageSeverity = severityFlags;
        debugMessengerCreateInfo.messageType = messageTypeFlags;
        debugMessengerCreateInfo.pfnUserCallback = &VulkanInitUtilities::DebugCallback;

        debugMessenger = instance.createDebugUtilsMessengerEXT(debugMessengerCreateInfo);
    }

    void Application::CreateSurface()
    {
        VkSurfaceKHR rawSurface;

        if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(*instance), nullptr, &rawSurface))
        {
            throw std::runtime_error("Failed to create window surface!");
        }

        surface = vk::raii::SurfaceKHR(instance, rawSurface);
    }

    void Application::PickPhysicalDevice()
    {
        auto devices = instance.enumeratePhysicalDevices();

        if (devices.empty())
        {
            throw std::runtime_error("failed to find GPU's with Vulkan support");
        }

        VulkanInitUtilities::FilterPhysicalDevices(devices,
            deviceExtensions);

        std::multimap<int, vk::raii::PhysicalDevice> deviceCandidates;

        for (const auto& device : devices)
        {
            bool hasGeomShader = false;
            uint32_t score = VulkanInitUtilities::GetPhysicalDeviceScore(device, hasGeomShader);

            if (!hasGeomShader)
            {
                continue;
            }

            deviceCandidates.insert(std::make_pair(score, device));
        }

        if (deviceCandidates.rbegin()->first > 0)
        {
            physicalDevice = deviceCandidates.rbegin()->second;
            std::cout << "Physical Device Found: " << physicalDevice.getProperties().deviceName << std::endl;
        } else
        {
            throw std::runtime_error("failed to find a suitable GPU");
        }
    }

    void Application::CreateLogicalDevice()
    {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        graphicsIndex = 0;
        uint32_t presentIndex = 0;

        VulkanInitUtilities::GetQueueFamilyIndices(queueFamilyProperties,
            graphicsIndex,
            presentIndex,
            physicalDevice,
            surface);

        bool graphicsCompatible = VulkanInitUtilities::IndexIsCompatible(graphicsIndex,
            queueFamilyProperties.size());

        bool presentCompatible = VulkanInitUtilities::IndexIsCompatible(presentIndex,
            queueFamilyProperties.size());

        if (!graphicsCompatible || !presentCompatible)
        {
            throw std::runtime_error("Could not find a queue for graphics or present -> terminating");
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {graphicsIndex, presentIndex};

        float queuePriority = 0.5f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain{};

        featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
        featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
        featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = true;
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        device = vk::raii::Device(physicalDevice, deviceCreateInfo);
        graphicsQueue = vk::raii::Queue(device, graphicsIndex, 0);
        presentQueue = vk::raii::Queue(device, presentIndex, 0);
    }

    void Application::CreateSwapchain()
    {
        vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
        std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(surface);

        swaphchainSurfaceFormat = SwapchainUtilities::ChooseSwapSurfaceFormat(availableFormats);
        swapchainExtent = SwapchainUtilities::ChooseSwapExtent(surfaceCapabilities, window);
        auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
        minImageCount = (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount) ? surfaceCapabilities.maxImageCount : minImageCount;

        uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
        swapchainCreateInfo.surface = *surface;
        swapchainCreateInfo.minImageCount = minImageCount;
        swapchainCreateInfo.imageFormat = swaphchainSurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = swaphchainSurfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = swapchainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchainCreateInfo.presentMode = SwapchainUtilities::ChooseSwapPresentMode(physicalDevice.getSurfacePresentModesKHR(*surface));
        swapchainCreateInfo.clipped = true;
        swapchainCreateInfo.oldSwapchain = nullptr;
        swapchain = vk::raii::SwapchainKHR(device, swapchainCreateInfo);
        swapchainImages = swapchain.getImages();
        swapchainImageFormat = swaphchainSurfaceFormat.format;
    }

    void Application::CleanupSwapchain()
    {
        swapChainImageViews.clear();
        swapchain = nullptr;
    }

    void Application::RecreateSwapchain()
    {
        int width, height = 0;
        SDL_GetWindowSizeInPixels(window, &width, &height);
        while (width == 0 || height == 0)
        {
            SDL_GetWindowSizeInPixels(window, &width, &height);
            SDL_WaitEvent(nullptr);
        }

        device.waitIdle();

        CleanupSwapchain();
        CreateSwapchain();
        CreateImageViews();
        CreateSyncObjects();
    }

    void Application::CreateImageViews()
    {
        swapChainImageViews.clear();

        vk::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = swapchainImageFormat;
        imageViewCreateInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

        for (vk::Image image : swapchainImages)
        {
            imageViewCreateInfo.image = image;
            swapChainImageViews.emplace_back(device, imageViewCreateInfo);
        }
    }

    void Application::CreateGraphicsPipeline()
    {
        auto shaderCode = AssetUtilities::ReadFile(AssetUtilities::GetAssetPath("assets/shaders/HelloTriangle.spv"));
        auto shaderModule = AssetUtilities::CreateShaderModule(shaderCode, device);

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

        pipelineLayout = vk::raii::PipelineLayout(device, layoutCreateInfo);

        vk::PipelineRenderingCreateInfo renderingCreateInfo{};
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &swapchainImageFormat;

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

        graphicsPipeline = vk::raii::Pipeline(device, nullptr, graphicsPipelineCreateInfo);
    }

    void Application::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex = graphicsIndex;

        commandPool = vk::raii::CommandPool(device, commandPoolCreateInfo);
    }

    void Application::CreateCommandBuffers()
    {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        commandBufferAllocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

        commandBuffers = vk::raii::CommandBuffers(device, commandBufferAllocateInfo);
    }

    void Application::CreateSyncObjects()
    {
        presentCompleteSemaphores.clear();
        renderFinishedSemaphores.clear();
        inFlightFences.clear();

        // assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

        for (size_t i = 0; i < swapchainImages.size(); i++)
        {
            renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
            inFlightFences.emplace_back(vk::raii::Fence(device, {vk::FenceCreateFlagBits::eSignaled}));
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
        renderingAttachmentInfo.imageView = swapChainImageViews[imageIndex];
        renderingAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        renderingAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        renderingAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        renderingAttachmentInfo.clearValue = clearColor;

        vk::RenderingInfo renderingInfo{};
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
        auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);

        if (fenceResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to wait for fence");
        }

        uint32_t imageIndex = 0;
        vk::Result result;

        try
        {
            auto acquireResult = swapchain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

            result = acquireResult.result;
            imageIndex = acquireResult.value;
        } catch (const vk::OutOfDateKHRError& e)
        {
            RecreateSwapchain();
            return;
        }

        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        device.resetFences(*inFlightFences[frameIndex]);
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

        graphicsQueue.submit(submitInfo, *inFlightFences[frameIndex]);

        vk::PresentInfoKHR presentInfoKHR{};
        presentInfoKHR.waitSemaphoreCount = 1;
        presentInfoKHR.pWaitSemaphores = &*renderFinishedSemaphores[imageIndex];
        presentInfoKHR.swapchainCount = 1;
        presentInfoKHR.pSwapchains = &*swapchain;
        presentInfoKHR.pImageIndices = &imageIndex;
        presentInfoKHR.pResults = nullptr;

        try
        {
            result = presentQueue.presentKHR(presentInfoKHR);
        } catch (const vk::OutOfDateKHRError& e)
        {
            RecreateSwapchain();
        }

        if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) || frameBufferResized)
        {
            frameBufferResized = false;
            RecreateSwapchain();
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
        barrier.image = swapchainImages[imageIndex];
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
