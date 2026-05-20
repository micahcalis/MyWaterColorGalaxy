#include "Rendering/Buffer/Image.hpp"
#include "Core/Application/Renderer/RenderGarbageCollector.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "ImageData.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/ComputeKernel.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "vulkan/vulkan.hpp"
#include "Core/Application/Managers/ImageAssetManager.hpp"

namespace Beer::Rendering
{
    Image::~Image()
    {
        if (allocator)
        {
            Core::RenderGarbageCollector::Push(
                [savedAllocator = this->allocator,
                    savedAllocation = this->allocation,
                    savedImageView = this->defaultView]() mutable {
                    savedAllocator->DestroyImage(savedAllocation);
                    savedAllocator->DestroyImageView(savedImageView);
                });
        }
    }

    Image Image::CreateImage2D(uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        vk::ImageAspectFlagBits aspectFlags,
        uint32_t layerCount,
        const Core::Device& device)
    {
        auto allocation = sharedAllocator->CreateImage2D(width,
            height,
            format,
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            usage,
            VMA_MEMORY_USAGE_AUTO,
            layerCount);

        auto defaultView = Core::ImageUtilities::CreateImageView(allocation.Image,
            vk::Format(format),
            aspectFlags,
            layerCount,
            true,
            device);

        ImageData data{};
        data.Extent = vk::Extent3D(width, height, 1);
        data.Format = format;
        data.AspectMask = aspectFlags;
        data.ArrayLayers = layerCount;
        data.Type = TextureType::TwoDim;

        return {allocation, defaultView, data};
    }

    Image Image::CreateImage3D(uint32_t width,
        uint32_t height,
        uint32_t depth,
        VkFormat format,
        VkImageUsageFlags usage,
        vk::ImageAspectFlagBits aspectFlags,
        const Core::Device& device)
    {
        auto allocation = sharedAllocator->CreateImage3D(width,
            height,
            depth,
            format,
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            usage,
            VMA_MEMORY_USAGE_AUTO);

        auto defaultView = Core::ImageUtilities::CreateImageView(allocation.Image,
            vk::Format(format),
            aspectFlags,
            1,
            false,
            device);

        ImageData data{};
        data.Extent = vk::Extent3D(width, height, depth);
        data.Format = format;
        data.AspectMask = aspectFlags;
        data.ArrayLayers = 1;
        data.Type = TextureType::ThreeDim;

        return {allocation, defaultView, data};
    }

    Image::Image(ImageAllocation allocation,
        VkImageView defaultView,
        ImageData data)
        : allocator(sharedAllocator), allocation(allocation), defaultView(defaultView), data(data)
    {
    }

    std::shared_ptr<Image> Image::GetAsset(const std::string& name)
    {
        return imageAssetManager->Get(name);
    }

    std::shared_ptr<Image> Image::Generate2D(uint32_t width,
        uint32_t height,
        VkFormat format,
        uint32_t layerCount,
        ComputeContext* computeContext,
        Threads threads,
        uint32_t kernelIndex)
    {
        std::shared_ptr<Rendering::Image> image = imageAssetManager->CreateEmpty2D(width,
            height,
            format,
            layerCount);

        imageAssetManager->GenerateFromEmpty(image,
            computeContext,
            threads,
            kernelIndex);

        return image;
    }

    std::shared_ptr<Image> Image::Generate3D(uint32_t width,
        uint32_t height,
        uint32_t depth,
        VkFormat format,
        ComputeContext* computeContext,
        Threads threads,
        uint32_t kernelIndex)
    {
        std::shared_ptr<Rendering::Image> image = imageAssetManager->CreateEmpty3D(width,
            height,
            depth,
            format);

        imageAssetManager->GenerateFromEmpty(image,
            computeContext,
            threads,
            kernelIndex);

        return image;
    }

    void Image::QueueTransitionLayout(const vk::Image image,
        const ImageData& imageData,
        const vk::raii::CommandBuffer& commandBuffer,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout)
    {
        vk::ImageMemoryBarrier barrier{};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;

        barrier.subresourceRange.aspectMask = imageData.AspectMask;
        barrier.subresourceRange.baseMipLevel = imageData.BaseMipLevel;
        barrier.subresourceRange.levelCount = imageData.MipLevels;
        barrier.subresourceRange.baseArrayLayer = imageData.BaseArrayLayer;
        barrier.subresourceRange.layerCount = imageData.ArrayLayers;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
        } else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eGeneral)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eComputeShader;
        } else if (oldLayout == vk::ImageLayout::eGeneral && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eComputeShader;
            destinationStage = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader;

        } else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(sourceStage,
            destinationStage,
            {},
            nullptr,
            nullptr,
            barrier);
    }

    void Image::QueueImageClear(vk::raii::CommandBuffer& commandBuffer,
        vk::ClearColorValue clearColor)
    {
        ImageData data = GetData();

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal);

        vk::ImageSubresourceRange range{};
        range.aspectMask = data.AspectMask;
        range.baseMipLevel = data.BaseMipLevel;
        range.levelCount = data.MipLevels;
        range.baseArrayLayer = data.BaseArrayLayer;
        range.layerCount = data.ArrayLayers;

        commandBuffer.clearColorImage(GetHandle(),
            vk::ImageLayout::eTransferDstOptimal,
            clearColor,
            {range});

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    void Image::QueueDepthClear(vk::raii::CommandBuffer& commandBuffer,
        vk::ClearDepthStencilValue clearValue)
    {
        ImageData data = GetData();

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal);

        vk::ImageSubresourceRange range{};
        range.aspectMask = data.AspectMask;
        range.baseMipLevel = data.BaseMipLevel;
        range.levelCount = data.MipLevels;
        range.baseArrayLayer = data.BaseArrayLayer;
        range.layerCount = data.ArrayLayers;

        commandBuffer.clearDepthStencilImage(GetHandle(),
            vk::ImageLayout::eTransferDstOptimal,
            clearValue,
            {range});

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    void Image::QueueImageGenerate(vk::raii::CommandBuffer& commandBuffer,
        Rendering::ComputeContext* computeContext,
        Rendering::Threads threads,
        uint32_t kernelIndex)
    {
        ImageData data = GetData();

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eGeneral);

        const ComputeKernel* kernel = computeContext->GetCompute()->GetKernel(kernelIndex);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, kernel->Pipeline);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
            computeContext->GetCompute()->GetPipelineLayout(),
            MaterialData::SET,
            {computeContext->GetDescriptorSet()},
            nullptr);

        if (threads.X == 0 || threads.Y == 0 || threads.Z == 0)
            throw std::runtime_error("Can't Dispatch Compute With 0 Thread Groups");

        commandBuffer.dispatch(threads.X, threads.Y, threads.Z);

        QueueTransitionLayout(GetHandle(),
            data,
            commandBuffer,
            vk::ImageLayout::eGeneral,
            vk::ImageLayout::eShaderReadOnlyOptimal);
    }

} // namespace Beer::Rendering
