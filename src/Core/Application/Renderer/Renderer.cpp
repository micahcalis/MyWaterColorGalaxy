#include "Core/Application/Renderer/Renderer.hpp"
#include "Core/Application/Managers/ImageAssetManager.hpp"
#include "Core/Application/Managers/MeshManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/FrameResource.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "Core/Application/Utilities/VulkanInitUtilities.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"
#include "Core/Application/Utilities/RendererUtilities.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Buffer/Image.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include "Rendering/UniformBufferObject.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <stdexcept>
#include <tiny_obj_loader.h>
#include "Rendering/Shader/Shader.hpp"

namespace Beer::Core
{
    const std::vector<char const*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
    constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    Renderer::~Renderer()
    {
        Rendering::Buffer::SetAllocator(nullptr);
        Rendering::Image::SetAllocator(nullptr);
    }

    void Renderer::InitializeVulkanInstances(SDL_Window* window)
    {
        this->window = window;
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface(window);
        device.Initialize(instance, surface);
        swapchain.InitializeSwapchain(window, surface, device);
        InitializeBuffers();
        vk::Format depthFormat;
        CreateDepthResources(depthFormat);
        CreateSemaphores();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            frameResources.emplace_back(&device);
        };

        CreateUniformBuffers();
        CreateUniformDescriptor();
        InitializeAssetManagers(depthFormat);

        LoadShader();
        LoadModel();
        CreateTextureImage();
    }

    void Renderer::PreDraw()
    {
        uploadManager->FlushQueue(frameResources[frameIndex]);
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
        vk::Format emptyFormat;
        CreateDepthResources(emptyFormat);
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
        {
            return;
        }

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
        VkSurfaceKHR rawSurface = nullptr;

        if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(*instance), nullptr, &rawSurface))
        {
            throw std::runtime_error("Failed to create windowManager.GetWindow() surface!");
        }

        surface = vk::raii::SurfaceKHR(instance, rawSurface);
    }

    void Renderer::InitializeBuffers()
    {
        bufferAllocator = std::make_shared<Rendering::BufferAllocator>(device, instance);
        Rendering::Buffer::SetAllocator(bufferAllocator);
        Rendering::Image::SetAllocator(bufferAllocator);

        uploadManager = std::make_unique<UploadManager>(bufferAllocator, device);

        descriptorAllocator = std::make_unique<Rendering::DescriptorAllocator>(MAX_FRAMES_IN_FLIGHT,
            &device);

        Rendering::UniformDescriptor::SetDescriptorAllocator(descriptorAllocator.get());
        Rendering::UniformDescriptor::SetFrameIndex(frameIndex);
    }

    void Renderer::InitializeAssetManagers(vk::Format depthFormat)
    {
        shaderManager = std::make_unique<ShaderManager>(
            &device,
            &swapchain,
            uniformDescriptor->GetLayout(),
            depthFormat,
            MAX_FRAMES_IN_FLIGHT);

        Rendering::Shader::SetShaderManager(shaderManager.get());

        meshManager = std::make_unique<MeshManager>(uploadManager.get());

        Rendering::Mesh::SetMeshManager(meshManager.get());

        imageAssetManager = std::make_unique<ImageAssetManager>(
            &device,
            bufferAllocator,
            uploadManager.get());

        Rendering::Image::SetImageAssetManager(imageAssetManager.get());

        samplerCache = std::make_unique<Rendering::SamplerCache>(device);
        Rendering::Sampler::SetSamplerCache(samplerCache.get());
    }

    void Renderer::CreateDepthResources(vk::Format& depthFormat)
    {
        depthFormat = ImageUtilities::FindDepthFormat(device);
        vk::Extent2D extent = swapchain.GetExtent();

        depthImage = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(extent.width,
                extent.height,
                VkFormat(depthFormat),
                VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                vk::ImageAspectFlagBits::eDepth,
                device));
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

    void Renderer::LoadShader()
    {
        shader = Rendering::Shader::Get("HelloTriangle");
        shader->PrintConfig();
    }

    void Renderer::LoadModel()
    {
        mesh = Rendering::Mesh::Get("MDL_VikingRoom");
    }

    void Renderer::CreateUniformBuffers()
    {
        uniformBuffers.clear();
        vk::DeviceSize bufferSize = sizeof(Rendering::UniformBufferObject);

        uniformBuffers.reserve(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto buffer = Rendering::Buffer::CreateUniform(bufferSize);

            uniformBuffers.emplace_back(std::move(buffer));
        }
    }

    void Renderer::CreateUniformDescriptor()
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings = {
            vk::DescriptorSetLayoutBinding(0,
                vk::DescriptorType::eUniformBuffer,
                1,
                vk::ShaderStageFlagBits::eVertex,
                nullptr),
            vk::DescriptorSetLayoutBinding(1,
                vk::DescriptorType::eCombinedImageSampler,
                1,
                vk::ShaderStageFlagBits::eFragment,
                nullptr)};

        uniformDescriptor = std::make_unique<Rendering::UniformDescriptor>(bindings);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            uniformDescriptor->UpdateBufferInfo(i,
                0,
                uniformBuffers[i],
                sizeof(Rendering::UniformBufferObject));
        }

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = 1;

        vk::DescriptorSetLayout rawLayout = uniformDescriptor->GetLayout();
        pipelineLayoutInfo.pSetLayouts = &rawLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        pipelineLayout = vk::raii::PipelineLayout(device.GetLogicalDevice(), pipelineLayoutInfo);
    }

    void Renderer::CreateTextureImage()
    {
        texture = std::make_shared<Rendering::Texture2D>(std::string("Tex_VikingRoom"));

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            uniformDescriptor->UpdateImageInfo(i, 1, texture.get());
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
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::ImageAspectFlagBits::eColor);

        CommandBufferUtilities::TransitionImageLayout(commandBuffer,
            depthImage->GetHandle(),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthAttachmentOptimal,
            vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
            vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
            vk::ImageAspectFlagBits::eDepth);

        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f);
        vk::RenderingAttachmentInfo colorAttachmentInfo = RendererUtilities::CreateColorAttachmentInfo(swapchain.GetImageView(imageIndex),
            clearColor);

        vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);
        vk::RenderingAttachmentInfo depthAttachmentInfo = RendererUtilities::CreateDepthAttachmentInfo(depthImage->GetDefaultView(), clearDepth);

        const vk::Extent2D& swapchainExtent = swapchain.GetExtent();

        vk::RenderingInfo renderingInfo = RendererUtilities::CreateRenderingInfo(swapchainExtent,
            colorAttachmentInfo,
            depthAttachmentInfo);

        commandBuffer.beginRendering(renderingInfo);

        commandBuffer.setViewport(0,
            vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f));

        commandBuffer.setScissor(0,
            vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, uniformDescriptor->GetSet(frameIndex), nullptr);

        CommandBufferUtilities::DrawMesh(commandBuffer, mesh.get(), shader.get(), Rendering::ShaderPassType::Opaque);
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
            vk::PipelineStageFlagBits2::eBottomOfPipe,
            vk::ImageAspectFlagBits::eColor);

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
        Rendering::UniformDescriptor::SetFrameIndex(frameIndex);
    }

    void Renderer::SetFrameBufferResized(const bool val) { frameBufferResized = val; }

    void Renderer::UpdateUniformBuffer(uint32_t frameIndex)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        const vk::Extent2D extent = swapchain.GetExtent();
        const float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);

        Rendering::UniformBufferObject ubo{}; /// Don't forget to add the f, 2.0 is a double instead of a float.
        ubo.objToWorld = rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        ubo.worldToView = glm::lookAt(glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        ubo.viewToClip = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
        ubo.viewToClip[1][1] *= -1;

        uniformBuffers[frameIndex].Upload(&ubo, sizeof(ubo));
    }
} // namespace Beer::Core
