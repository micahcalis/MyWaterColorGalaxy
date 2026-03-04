#pragma once

#include "Rendering/Buffer/ImageAllocation.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Buffer/ImageAllocation.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Utilities/macros.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    class ImageAssetManager;
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
        vk::Extent3D extent;
        VkFormat format;

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
            const Core::Device& device);

        static void SetImageAssetManager(Core::ImageAssetManager* imageAssetManager)
        {
            Image::imageAssetManager = imageAssetManager;
        }

        static std::shared_ptr<Image> GetAsset(const std::string& name);

        [[nodiscard]] VkImage GetHandle() const { return allocation.Image; }
        VkImageView GetDefaultView() const { return defaultView; }
        vk::Extent3D GetExtent() const { return extent; }
        VkFormat GetFormat() const { return format; }

        void QueueTransitionLayout(const vk::Image image,
            const vk::raii::CommandBuffer& commandBuffer,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout);

        NO_COPY(Image);
        DEFAULT_MOVE(Image);

    private:
        Image(ImageAllocation allocation,
            VkImageView defaultView,
            vk::Extent3D extent,
            VkFormat format);
    };
} // namespace Beer::Rendering
