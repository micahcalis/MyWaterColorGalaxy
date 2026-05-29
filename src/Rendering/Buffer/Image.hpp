#pragma once

#include "ImageData.hpp"
#include "Rendering/Buffer/ImageAllocation.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Buffer/ImageAllocation.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Utilities/macros.hpp"
#include "Rendering/Compute/Threads.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    class ImageAssetManager;
}

namespace Beer::Rendering
{
    class ComputeContext;
}

namespace Beer::Rendering
{
    class Image
    {
    private:
        // this allocator stuff is pretty cursed, but C objects have weird behaviour so this is fine for now
        inline static std::shared_ptr<BufferAllocator> sharedAllocator = nullptr;
        inline static Core::ImageAssetManager* imageAssetManager = nullptr;

        std::shared_ptr<BufferAllocator> allocator;
        ImageAllocation allocation;
        VkImageView defaultView;
        ImageData data;

    public:
        ~Image();

        static void SetAllocator(std::shared_ptr<BufferAllocator> allocator)
        {
            Image::sharedAllocator = allocator;
        }

        static Image CreateImage2D(uint32_t width,
            uint32_t height,
            VkFormat format,
            VkImageUsageFlags usage,
            vk::ImageAspectFlagBits aspectFlags,
            uint32_t layerCount,
            const Core::Device& device);

        static Image CreateImage3D(uint32_t width,
            uint32_t height,
            uint32_t depth,
            VkFormat format,
            VkImageUsageFlags usage,
            vk::ImageAspectFlagBits aspectFlags,
            const Core::Device& device);

        static void SetImageAssetManager(Core::ImageAssetManager* imageAssetManager)
        {
            Image::imageAssetManager = imageAssetManager;
        }

        static std::shared_ptr<Image> GetAsset(const std::string& name);

        static std::shared_ptr<Image> Generate2D(uint32_t width,
            uint32_t height,
            VkFormat format,
            uint32_t layerCount,
            ComputeContext* computeContext,
            Threads threads,
            uint32_t kernelIndex);

        static std::shared_ptr<Image> Generate3D(uint32_t width,
            uint32_t height,
            uint32_t depth,
            VkFormat format,
            ComputeContext* computeContext,
            Threads threads,
            uint32_t kernelIndex);

        [[nodiscard]] VkImage GetHandle() const { return allocation.Image; }
        VkImageView GetDefaultView() const { return defaultView; }
        vk::Extent3D GetExtent() const { return data.Extent; }
        VkFormat GetFormat() const { return data.Format; }
        ImageData GetData() const { return data; }

        void QueueTransitionLayout(const vk::Image image,
            const ImageData& imageData,
            const vk::raii::CommandBuffer& commandBuffer,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout);

        void QueueImageClear(vk::raii::CommandBuffer& commandBuffer,
            vk::ClearColorValue clearColor);

        void QueueDepthClear(vk::raii::CommandBuffer& commandBuffer,
            vk::ClearDepthStencilValue clearValue);

        void QueueImageGenerate(vk::raii::CommandBuffer& commandBuffer,
            Rendering::ComputeContext* computeContext,
            Rendering::Threads threads,
            uint32_t kernelIndex);

        NO_COPY(Image);
        DEFAULT_MOVE(Image);

    private:
        Image(ImageAllocation allocation,
            VkImageView defaultView,
            ImageData data);
    };
} // namespace Beer::Rendering
