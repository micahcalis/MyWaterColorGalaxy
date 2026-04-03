#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL_video.h>
#include "Core/Application/Managers/ComputeManager.hpp"
#include "Core/Application/Managers/ImageAssetManager.hpp"
#include "Core/Application/Managers/MeshManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include <memory>
#include <vector>
#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Buffer/Image.hpp"
#include "Rendering/Pipeline/RenderPipeline.hpp"
#include "Rendering/RenderPasses/RenderPassPool.hpp"
#include "Rendering/Sampler/SamplerCache.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Rendering/Mesh./Mesh.hpp"
#include "Core/Application/Managers/ShaderManager.hpp"
#include "Rendering/Uniforms/DescriptorAllocator.hpp"
#include "System/Drawing/RenderRegister.hpp"

namespace Beer::Core
{
    class Renderer
    {
    private:
        SDL_Window* window;

        vk::raii::Instance instance = nullptr;

        vk::raii::Context context;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
        vk::raii::SurfaceKHR surface = nullptr;
        Device device{};

        std::unique_ptr<Swapchain> swapchain = nullptr;

        std::shared_ptr<Rendering::Image> depthImage = nullptr;

        std::vector<FrameResource> frameResources;
        std::vector<vk::raii::Semaphore> swapchainSemaphores;

        std::shared_ptr<Rendering::BufferAllocator> bufferAllocator = nullptr;
        std::shared_ptr<UploadManager> uploadManager = nullptr;
        std::unique_ptr<Rendering::DescriptorAllocator> descriptorAllocator = nullptr;

        std::unique_ptr<ShaderManager> shaderManager = nullptr;
        std::unique_ptr<ComputeManager> computeManager = nullptr;
        std::unique_ptr<MeshManager> meshManager = nullptr;
        std::unique_ptr<ImageAssetManager> imageAssetManager = nullptr;
        std::unique_ptr<Rendering::SamplerCache> samplerCache = nullptr;
        std::unique_ptr<System::RenderRegister> renderRegister = nullptr;

        std::unique_ptr<Rendering::RenderPassPool> renderPassPool = nullptr;
        std::unique_ptr<Rendering::RenderPipeline> renderPipeline = nullptr;

        int frameIndex = 0;
        bool frameBufferResized = false;

    public:
        ~Renderer();
        void InitializeVulkanInstances(SDL_Window* window);
        void PreDraw();
        void Draw();
        void HandleWindowResize();
        void SetFrameBufferResized(bool val);
        void Present(uint32_t imageIndex);

        [[nodiscard]] const vk::raii::Context& GetContext() const;
        [[nodiscard]] const vk::raii::Instance& GetInstance() const;
        [[nodiscard]] const vk::raii::DebugUtilsMessengerEXT& GetDebugMessenger() const;
        [[nodiscard]] const vk::raii::SurfaceKHR& GetSurface() const;
        [[nodiscard]] const Device& GetDevice() const;

        Swapchain* GetSwapchain();
        bool& GetFrameBufferResized();

    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface(SDL_Window* window);
        void CreateDepthResources(vk::Format& depthFormat);
        void SetScreenGlobal(vk::Format& depthFormat);
        void CreateSemaphores();
        void InitializeBuffers();
        void InitializeAssetManagers(vk::Format depthFormat);
        void InitializeRenderPipeline();
        void UpdateGlobals();

    private:
        inline static Swapchain* mainSwapchain;

    public:
        static void SetMainSwapchain(Swapchain* swapchain)
        {
            mainSwapchain = swapchain;
        }

        static vk::Extent2D GetScreenExtent();
    };

} // namespace Beer::Core
