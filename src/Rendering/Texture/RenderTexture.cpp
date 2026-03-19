#include "Rendering/Texture/RenderTexture.hpp"
#include "ReallocationFlags.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncState.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include <memory>

namespace Beer::Rendering
{
    RenderTexture::RenderTexture(std::shared_ptr<Image> image,
        vk::Filter filter,
        vk::SamplerAddressMode tiling)
        : ITexture(image, Sampler::Get(filter, tiling))
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
} // namespace Beer::Rendering
