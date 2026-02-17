#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL_video.h>
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Renderer/PipelineCache.hpp"
#include <memory>
#include <vector>
#include "Core/Application/Renderer/FrameResource.hpp"
#include "Core/Application/Renderer/BufferAllocator.hpp"
#include "Rendering/Buffer/Buffer.hpp"

namespace Beer::Core
{
    class Renderer
    {
    private:
        SDL_Window* window;

        vk::raii::Instance instance = nullptr;

        Device device{};

        vk::raii::Context context;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
        vk::raii::SurfaceKHR surface = nullptr;

        Swapchain swapchain{};

        std::unique_ptr<PipelineCache> pipelineCache = nullptr;
        std::vector<FrameResource> frameResources;
        std::vector<vk::raii::Semaphore> swapchainSemaphores;

        std::shared_ptr<BufferAllocator> bufferAllocator = nullptr;

        std::unique_ptr<Rendering::Buffer> vertexBuffer;
        std::unique_ptr<Rendering::Buffer> indexBuffer;

        std::vector<Rendering::Buffer> uniformBuffers;

        int frameIndex = 0;
        bool frameBufferResized = false;

        vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::DescriptorPool descriptorPool = nullptr;
        std::vector<vk::raii::DescriptorSet> descriptorSets;

    public:
        void InitializeVulkanInstances(SDL_Window* window);
        void Draw();
        void HandleWindowResize();
        void SetFrameBufferResized(bool val);

        [[nodiscard]] const vk::raii::Context& GetContext() const;
        [[nodiscard]] const vk::raii::Instance& GetInstance() const;
        [[nodiscard]] const vk::raii::DebugUtilsMessengerEXT& GetDebugMessenger() const;
        [[nodiscard]] const vk::raii::SurfaceKHR& GetSurface() const;
        [[nodiscard]] const Device& GetDevice() const;

        Swapchain& GetSwapchain();
        bool& GetFrameBufferResized();

    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface(SDL_Window* window);
        void CreateSemaphores();
        void CreateDesciptorSetLayout();
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        void BeginFrame(FrameResource& frameResource, const uint32_t& imageIndex);
        void EndFrame(FrameResource& frameResource, const uint32_t& imageIndex);
        void UpdateUniformBuffer(uint32_t frameIndex);
    };
} // namespace Beer::Core
