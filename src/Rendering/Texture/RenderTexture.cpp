#include "Rendering/Texture/RenderTexture.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "ReallocationFlags.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncState.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    RenderTexture::RenderTexture(const std::string& name,
        std::shared_ptr<Image> image,
        vk::Filter filter,
        vk::SamplerAddressMode tiling)
        : name(name), ITexture(image, Sampler::Get(filter, tiling))
    {
        type = RenderResourceType::Image;
        syncState = std::make_unique<ImageSyncState>(image.get());
    }

    void RenderTexture::SetImage(std::shared_ptr<Image> image)
    {
        this->image = std::move(image);
        syncState = std::make_unique<ImageSyncState>(image.get());
    }

    void RenderTexture::SetSampler(vk::Filter filter, vk::SamplerAddressMode tiling)
    {
        sampler = Sampler::Get(filter, tiling);
    }

    ReallocationMask RenderTexture::GetAllocationMask(uint32_t width,
        uint32_t height,
        VkFormat format,
        vk::Filter filter,
        vk::SamplerAddressMode tiling)
    {
        uint32_t bits = 0;

        ImageData imageData = image->GetData();

        if (imageData.Extent.width != width
            || imageData.Extent.height != height
            || imageData.Format != format)
        {
            bits |= REALLOC_IMAGE_BITS;
        }

        SamplerData samplerData = sampler->GetData();

        if (samplerData.Filter != filter
            || samplerData.Tiling != tiling)
        {
            bits |= REALLOC_SAMPLER_BITS;
        }

        return ReallocationMask(bits);
    }

    std::unique_ptr<ISyncBarrier> RenderTexture::GetBarrier(const ResourceAction action)
    {
        std::unique_ptr<ImageSyncBarrier> barrier = std::make_unique<ImageSyncBarrier>(
            GetSyncState());

        barrier->GenerateBarrier(action);
        return barrier;
    }

    vk::RenderingAttachmentInfo RenderTexture::GetAttachmentInfo(const ResetOperator& resetOperator,
        bool& isDepth)
    {
        isDepth = Core::ImageUtilities::IsDepthFormat(static_cast<vk::Format>(image->GetData().Format));

        vk::RenderingAttachmentInfo attachmentInfo{};
        attachmentInfo.imageView = image->GetDefaultView();

        attachmentInfo.imageLayout = isDepth
            ? vk::ImageLayout::eDepthStencilAttachmentOptimal
            : vk::ImageLayout::eColorAttachmentOptimal;

        attachmentInfo.loadOp = resetOperator.LoadOp;
        attachmentInfo.storeOp = resetOperator.StoreOp;
        attachmentInfo.clearValue = resetOperator.ClearValue;

        return attachmentInfo;
    }
} // namespace Beer::Rendering
