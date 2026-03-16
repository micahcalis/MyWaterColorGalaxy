#include "Core/Application/Renderer/Renderer.hpp"
#include "Core/Application/Managers/ImageAssetManager.hpp"
#include "Core/Application/Managers/MeshManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/DrawCallPool.hpp"
#include "Core/Application/Renderer/FrameResource.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "Core/Application/Utilities/VulkanInitUtilities.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"
#include "Core/Application/Utilities/RendererUtilities.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Buffer/Image.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Drawing/ContextMask.hpp"
#include "System/Drawing/DrawRequest.hpp"
#include "System/Drawing/Layer.hpp"
#include "System/Drawing/LayerMask.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Light/ILight.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <tiny_obj_loader.h>
#include "Rendering/Shader/Shader.hpp"
#include "System/Camera/Camera.hpp"

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
        device.GetLogicalDevice().waitIdle();

        Rendering::Texture2D::ResetFallbackTexture();
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
        swapchain = std::make_unique<Swapchain>();
        swapchain->InitializeSwapchain(window, surface, device);
        SetMainSwapchain(swapchain.get());
        InitializeBuffers();
        vk::Format depthFormat;
        CreateDepthResources(depthFormat);
        CreateSemaphores();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            frameResources.emplace_back(&device);
        };

        InitializeAssetManagers(depthFormat);
    }

    void Renderer::PreDraw()
    {
        Rendering::Material::UpdateDirtyMaterials();
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
        bool resize = RendererUtilities::AcquireNextImage(swapchain.get(), frameResource, imageIndex);

        if (!resize)
            return;

        if (System::Camera::Main() == nullptr)
            return;

        frameResource.Reset();

        BeginFrame(frameResource, imageIndex);
        EndFrame(frameResource, imageIndex);
    }

    void Renderer::HandleWindowResize()
    {
        device.GetLogicalDevice().waitIdle();
        swapchain->RecreateSwapchain(window, surface, device);
        CreateSemaphores();
        vk::Format emptyFormat;
        CreateDepthResources(emptyFormat);
    }

    const vk::raii::Context& Renderer::GetContext() const { return context; }
    const vk::raii::Instance& Renderer::GetInstance() const { return instance; }
    const vk::raii::DebugUtilsMessengerEXT& Renderer::GetDebugMessenger() const { return debugMessenger; }
    const vk::raii::SurfaceKHR& Renderer::GetSurface() const { return surface; }
    const Device& Renderer::GetDevice() const { return device; }
    Swapchain* Renderer::GetSwapchain() { return swapchain.get(); }
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
            swapchain.get(),
            depthFormat,
            MAX_FRAMES_IN_FLIGHT);

        Rendering::Shader::SetShaderManager(shaderManager.get());

        meshManager = std::make_unique<MeshManager>(uploadManager.get());

        Rendering::Mesh::SetMeshManager(meshManager.get());

        samplerCache = std::make_unique<Rendering::SamplerCache>(device);
        Rendering::Sampler::SetSamplerCache(samplerCache.get());

        imageAssetManager = std::make_unique<ImageAssetManager>(
            &device,
            bufferAllocator,
            uploadManager.get());

        Rendering::Image::SetImageAssetManager(imageAssetManager.get());

        renderRegister = std::make_unique<System::RenderRegister>();
        System::RenderRegister::SetRenderRegister(renderRegister.get());
    }

    void Renderer::CreateDepthResources(vk::Format& depthFormat)
    {
        depthFormat = ImageUtilities::FindDepthFormat(device);
        vk::Extent2D extent = swapchain->GetExtent();

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
        size_t imageCount = swapchain->GetSwapchainCount();

        vk::SemaphoreCreateInfo semaphoreInfo{};
        for (size_t i = 0; i < imageCount; i++)
        {
            swapchainSemaphores.emplace_back(device.GetLogicalDevice(), semaphoreInfo);
        }
    }

    void Renderer::BeginFrame(FrameResource& frameResource, const uint32_t& imageIndex)
    {
        vk::CommandBuffer commandBuffer = frameResource.GetCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffer.begin(beginInfo);

        CommandBufferUtilities::TransitionImageLayout(commandBuffer,
            swapchain->GetImage(imageIndex),
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
        vk::RenderingAttachmentInfo colorAttachmentInfo = RendererUtilities::CreateColorAttachmentInfo(swapchain->GetImageView(imageIndex),
            clearColor);

        vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);
        vk::RenderingAttachmentInfo depthAttachmentInfo = RendererUtilities::CreateDepthAttachmentInfo(depthImage->GetDefaultView(), clearDepth);

        const vk::Extent2D& swapchainExtent = swapchain->GetExtent();

        vk::RenderingInfo renderingInfo = RendererUtilities::CreateRenderingInfo(swapchainExtent,
            colorAttachmentInfo,
            depthAttachmentInfo);

        commandBuffer.beginRendering(renderingInfo);

        commandBuffer.setViewport(0,
            vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f));

        commandBuffer.setScissor(0,
            vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));

        UpdateGlobals();
        Rendering::Shader::Globals()->Bind(commandBuffer);

        System::DrawRequest drawRequest = System::DrawRequest(commandBuffer,
            Rendering::ShaderPassType::Opaque,
            System::ContextMask(System::CTXT_GALAXY_BITS),
            System::LayerMask(System::LAYER_ALL_BITS));

        DrawCallPool drawPool = renderRegister->GetDrawCallPool(drawRequest);
        drawPool.BindDrawCalls();
    }

    void Renderer::EndFrame(FrameResource& frameResource, const uint32_t& imageIndex)
    {
        vk::CommandBuffer commandBuffer = frameResource.GetCommandBuffer();
        vk::Semaphore signalSemaphore = *swapchainSemaphores[imageIndex];

        commandBuffer.endRendering();

        CommandBufferUtilities::TransitionImageLayout(commandBuffer,
            swapchain->GetImage(imageIndex),
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe,
            vk::ImageAspectFlagBits::eColor);

        commandBuffer.end();

        auto waitMask = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo = RendererUtilities::CreateSubmitInfo(frameResource, commandBuffer, &waitMask);
        submitInfo.setSignalSemaphores(signalSemaphore);
        device.GetGraphicsQueue().submit(submitInfo, *frameResource.GetInFlightFence());
        vk::PresentInfoKHR presentInfo = RendererUtilities::CreatePresentInfo(frameResource, swapchain.get(), imageIndex);
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

    void Renderer::UpdateGlobals()
    {
        const vk::Extent2D extent = swapchain->GetExtent();
        Rendering::Shader::Globals()->SetScreen(static_cast<float>(extent.width), static_cast<float>(extent.height));
        System::Camera::Main()->BindToShaders();
        System::ILight* light = System::ILight::Main();
        Rendering::Shader::Globals()->SetMainLight(light->GetPosition(), light->GetDirectColor());
        Rendering::Shader::Globals()->SetAmbientLight(light->GetShadowColor(), light->GetAmbientColor());

        Rendering::Shader::Globals()->Update();
    }

    vk::Extent2D Renderer::GetScreenExtent()
    {
        if (mainSwapchain == nullptr)
            return vk::Extent2D(0);

        return mainSwapchain->GetExtent();
    }
} // namespace Beer::Core
