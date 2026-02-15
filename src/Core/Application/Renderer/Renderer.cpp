#include "Core/Application/Renderer/Renderer.hpp"
#include "Core/Application/Renderer/FrameResource.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "Core/Application/Utilities/VulkanInitUtilities.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"
#include "Core/Application/Utilities/RendererUtilities.hpp"
#include "Core/Application/Renderer/PipelineKey.hpp"
#include "Core/Application/Renderer/PipelineData.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <memory>
#include "Rendering/Vertex.hpp"

namespace Beer::Core
{
    const std::vector<char const*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
    constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

    constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    constexpr std::string_view HELLO_TRIANGLE = "HelloTriangle";
    const std::vector<Rendering::Vertex> helloTriangleVertices = {
        Rendering::Vertex{glm::vec2(0.0, -0.5), glm::vec3(1.0, 1.0, 1.0)},
        Rendering::Vertex{glm::vec2(0.5, 0.5), glm::vec3(0.0, 1.0, 0.0)},
        Rendering::Vertex{glm::vec2(-0.5, 0.5), glm::vec3(0.0, 0.0, 1.0)}};

    void Renderer::InitializeVulkanInstances(SDL_Window* window)
    {
        this->window = window;
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface(window);
        device.Initialize(instance, surface);
        swapchain.InitializeSwapchain(window, surface, device);
        CreateSemaphores();
        pipelineCache = std::make_unique<PipelineCache>(device.GetLogicalDevice(), swapchain);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            frameResources.emplace_back(FrameResource(&device));
        }

        CreateVertexBuffer();
    }

    void Renderer::Draw()
    {
        auto& frameResource = frameResources[frameIndex];
        auto fenceResult = device.GetLogicalDevice().waitForFences(*frameResource.GetInFlightFence(),
            vk::True,
            UINT64_MAX);

        if (fenceResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to wait for fence");
        }

        uint32_t imageIndex = 0;
        bool resize = RendererUtilities::AcquireNextImage(swapchain, frameResource, imageIndex);

        if (!resize)
        {
            return;
        }

        frameResource.Reset();

        BeginFrame(frameResource, imageIndex);
        EndFrame(frameResource, imageIndex);
    }

    void Renderer::HandleWindowResize()
    {
        device.GetLogicalDevice().waitIdle();
        swapchain.RecreateSwapchain(window, surface, device);
        CreateSemaphores();
    }

    const vk::raii::Context& Renderer::GetContext() const { return context; }
    const vk::raii::Instance& Renderer::GetInstance() const { return instance; }
    const vk::raii::DebugUtilsMessengerEXT& Renderer::GetDebugMessenger() const { return debugMessenger; }
    const vk::raii::SurfaceKHR& Renderer::GetSurface() const { return surface; }
    const Device& Renderer::GetDevice() const { return device; }
    Swapchain& Renderer::GetSwapchain() { return swapchain; }
    bool& Renderer::GetFrameBufferResized() { return frameBufferResized; }

    void Renderer::CreateInstance()
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

    void Renderer::SetupDebugMessenger()
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

    void Renderer::CreateSurface(SDL_Window* window)
    {
        VkSurfaceKHR rawSurface;

        if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(*instance), nullptr, &rawSurface))
        {
            throw std::runtime_error("Failed to create windowManager.GetWindow() surface!");
        }

        surface = vk::raii::SurfaceKHR(instance, rawSurface);
    }

    void Renderer::CreateSemaphores()
    {
        swapchainSemaphores.clear();
        size_t imageCount = swapchain.GetSwapchainCount();

        vk::SemaphoreCreateInfo semaphoreInfo{};
        for (size_t i = 0; i < imageCount; i++)
        {
            swapchainSemaphores.emplace_back(device.GetLogicalDevice(), semaphoreInfo);
        }
    }

    void Renderer::CreateVertexBuffer()
    {
        vk::DeviceSize bufferSize = sizeof(helloTriangleVertices[0]) * helloTriangleVertices.size();

        vk::BufferCreateInfo stagingInfo{};
        stagingInfo.size = bufferSize;
        stagingInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
        stagingInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::raii::Buffer stagingBuffer(device.GetLogicalDevice(), stagingInfo);
        vk::MemoryRequirements memRequirementsStaging = stagingBuffer.getMemoryRequirements();

        vk::MemoryAllocateInfo memoryAllocateInfoStaging{};
        memoryAllocateInfoStaging.allocationSize = memRequirementsStaging.size;
        memoryAllocateInfoStaging.memoryTypeIndex = RendererUtilities::FindMemoryType(memRequirementsStaging.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            device.GetPhysicalDevice());

        vk::raii::DeviceMemory stagingBufferMemory(device.GetLogicalDevice(), memoryAllocateInfoStaging);

        stagingBuffer.bindMemory(stagingBufferMemory, 0);
        void* dataStaging = stagingBufferMemory.mapMemory(0, stagingInfo.size);
        memcpy(dataStaging, helloTriangleVertices.data(), stagingInfo.size);
        stagingBufferMemory.unmapMemory();

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.size = bufferSize;
        bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vertexBuffer = vk::raii::Buffer(device.GetLogicalDevice(), bufferInfo);

        vk::MemoryRequirements memRequirements = vertexBuffer.getMemoryRequirements();

        vk::MemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.allocationSize = memRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = RendererUtilities::FindMemoryType(memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            device.GetPhysicalDevice());

        vertexBufferMemory = vk::raii::DeviceMemory(device.GetLogicalDevice(), memoryAllocateInfo);
        vertexBuffer.bindMemory(vertexBufferMemory, 0);
        RendererUtilities::CopyBuffer(stagingBuffer, vertexBuffer, stagingInfo.size, device, frameResources[frameIndex]);

        // RendererUtilities::CreateBuffer(bufferSize,
        //     vk::BufferUsageFlagBits::eVertexBuffer,
        //     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        //     device,
        //     vertexBuffer,
        //     vertexBufferMemory);

        // RendererUtilities::MapVertices(vertexBuffer,
        //     vertexBufferMemory,
        //     helloTriangleVertices,
        //     bufferSize);
    }

    void Renderer::BeginFrame(FrameResource& frameResource, const uint32_t& imageIndex)
    {
        vk::CommandBuffer commandBuffer = frameResource.GetCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffer.begin(beginInfo);

        CommandBufferUtilities::TransitionImageLayout(commandBuffer,
            swapchain.GetImage(imageIndex),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput);

        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f);

        vk::RenderingAttachmentInfo colorAttachmentInfo = RendererUtilities::CreateColorAttachmentInfo(swapchain.GetImageView(imageIndex),
            clearColor);

        const vk::Extent2D& swapchainExtent = swapchain.GetExtent();

        vk::RenderingInfo renderingInfo = RendererUtilities::CreateRenderingInfo(swapchainExtent,
            colorAttachmentInfo);

        commandBuffer.beginRendering(renderingInfo);

        commandBuffer.setViewport(0,
            vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f));

        commandBuffer.setScissor(0,
            vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));

        // HARDCODED DRAW BLOCK : EXTENSION NECESSARY!!!
        PipelineData pipelineData{};
        pipelineData.ShaderName = HELLO_TRIANGLE;
        pipelineData.ShaderPath = AssetUtilities::GetShaderPath(std::string(HELLO_TRIANGLE));
        const vk::raii::Pipeline& pipeline = pipelineCache->GetPipeline(PipelineKey(std::string(HELLO_TRIANGLE)),
            pipelineData);

        CommandBufferUtilities::DrawCall(commandBuffer, pipeline, vertexBuffer);
    }

    void Renderer::EndFrame(FrameResource& frameResource, const uint32_t& imageIndex)
    {
        vk::CommandBuffer commandBuffer = frameResource.GetCommandBuffer();
        vk::Semaphore signalSemaphore = *swapchainSemaphores[imageIndex];

        commandBuffer.endRendering();

        CommandBufferUtilities::TransitionImageLayout(commandBuffer,
            swapchain.GetImage(imageIndex),
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe);

        commandBuffer.end();

        auto waitMask = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo = RendererUtilities::CreateSubmitInfo(frameResource, commandBuffer, &waitMask);
        submitInfo.setSignalSemaphores(signalSemaphore);
        device.GetGraphicsQueue().submit(submitInfo, *frameResource.GetInFlightFence());
        vk::PresentInfoKHR presentInfo = RendererUtilities::CreatePresentInfo(frameResource, swapchain, imageIndex);
        presentInfo.setWaitSemaphores(signalSemaphore);

        vk::Result result = RendererUtilities::Queue_PresentKHR_NoExcept(device.GetPresentQueue(), presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR)
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

    void Renderer::SetFrameBufferResized(const bool val) { frameBufferResized = val; }
} // namespace Beer::Core
