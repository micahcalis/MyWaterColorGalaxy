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
#include "Rendering/UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

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
        Rendering::Vertex{glm::vec2(-0.5, -0.5), glm::vec3(1.0, 0.0, 0.0)},
        Rendering::Vertex{glm::vec2(0.5, -0.5), glm::vec3(1.0, 1.0, 1.0)},
        Rendering::Vertex{glm::vec2(0.5, 0.5), glm::vec3(0.0, 1.0, 0.0)},
        Rendering::Vertex{glm::vec2(-0.5, 0.5), glm::vec3(0.0, 0.0, 1.0)}};

    const std::vector<uint16_t> helloTriangleIndices = {
        0, 1, 2, 2, 3, 0};

    void Renderer::InitializeVulkanInstances(SDL_Window* window)
    {
        this->window = window;
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface(window);
        device.Initialize(instance, surface);
        swapchain.InitializeSwapchain(window, surface, device);
        CreateSemaphores();
        CreateUniformBuffers();
        CreateDesciptorSetLayout();
        CreateDescriptorPool();
        CreateDescriptorSets();
        pipelineCache = std::make_unique<PipelineCache>(device.GetLogicalDevice(), swapchain, descriptorSetLayout);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            frameResources.emplace_back(FrameResource(&device));
        }

        CreateVertexBuffer();
        CreateIndexBuffer();
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

    void Renderer::CreateIndexBuffer()
    {
        vk::DeviceSize bufferSize = sizeof(helloTriangleIndices[0]) * helloTriangleIndices.size();

        vk::raii::Buffer stagingBuffer({});
        vk::raii::DeviceMemory stagingBufferMemory({});

        RendererUtilities::CreateBuffer(bufferSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            device,
            stagingBuffer,
            stagingBufferMemory);

        void* data = stagingBufferMemory.mapMemory(0, bufferSize);
        memcpy(data, helloTriangleIndices.data(), (size_t)bufferSize);
        stagingBufferMemory.unmapMemory();

        RendererUtilities::CreateBuffer(bufferSize,
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            device,
            indexBuffer,
            indexBufferMemory);

        RendererUtilities::CopyBuffer(stagingBuffer,
            indexBuffer,
            bufferSize,
            device,
            frameResources[frameIndex]);
    }

    void Renderer::CreateDesciptorSetLayout()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding(0,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex,
            nullptr);

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        descriptorSetLayout = vk::raii::DescriptorSetLayout(device.GetLogicalDevice(), layoutInfo);

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &*descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        pipelineLayout = vk::raii::PipelineLayout(device.GetLogicalDevice(), pipelineLayoutInfo);
    }

    void Renderer::CreateUniformBuffers()
    {
        uniformBuffers.clear();
        uniformBuffersMemory.clear();
        uniformBuffersMapped.clear();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DeviceSize bufferSize = sizeof(Rendering::UniformBufferObject);
            vk::raii::Buffer buffer({});
            vk::raii::DeviceMemory bufferMemory({});

            RendererUtilities::CreateBuffer(bufferSize,
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible,
                device,
                buffer,
                bufferMemory);

            uniformBuffers.emplace_back(std::move(buffer));
            uniformBuffersMemory.emplace_back(std::move(bufferMemory));
            uniformBuffersMapped.emplace_back(uniformBuffersMemory[i].mapMemory(0, bufferSize));
        }
    }

    void Renderer::CreateDescriptorPool()
    {
        vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT);

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;

        descriptorPool = vk::raii::DescriptorPool(device.GetLogicalDevice(), poolInfo);
    }

    void Renderer::CreateDescriptorSets()
    {
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
        allocateInfo.pSetLayouts = layouts.data();

        descriptorSets.clear();
        descriptorSets = device.GetLogicalDevice().allocateDescriptorSets(allocateInfo);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(Rendering::UniformBufferObject);

            vk::WriteDescriptorSet descriptorWrite{};
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrite.pBufferInfo = &bufferInfo;

            device.GetLogicalDevice().updateDescriptorSets(descriptorWrite, {});
        }
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

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, *descriptorSets[frameIndex], nullptr);

        // HARDCODED DRAW BLOCK : EXTENSION NECESSARY!!!
        PipelineData pipelineData{};
        pipelineData.ShaderName = HELLO_TRIANGLE;
        pipelineData.ShaderPath = AssetUtilities::GetShaderPath(std::string(HELLO_TRIANGLE));
        const vk::raii::Pipeline& pipeline = pipelineCache->GetPipeline(PipelineKey(std::string(HELLO_TRIANGLE)),
            pipelineData);

        CommandBufferUtilities::DrawIndexedCall(commandBuffer, pipeline, vertexBuffer, indexBuffer, helloTriangleIndices.size());
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

        UpdateUniformBuffer(frameIndex);

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

    void Renderer::UpdateUniformBuffer(uint32_t frameIndex)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        const vk::Extent2D extent = swapchain.GetExtent();
        const float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);

        Rendering::UniformBufferObject ubo{};
        ubo.objToWorld = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        ubo.worldToView = glm::lookAt(glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0));
        ubo.viewToClip = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
        ubo.viewToClip[1][1] *= -1;

        memcpy(uniformBuffersMapped[frameIndex], &ubo, sizeof(ubo));
    }
} // namespace Beer::Core
