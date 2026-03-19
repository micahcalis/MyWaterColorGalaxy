#pragma once

#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncState.hpp"
#include "Rendering/Texture/ReallocationFlags.hpp"
#include "Rendering/Pipeline/Frame//Resource/IRenderResource.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class RenderTexture : public ITexture
        , public IRenderResource
    {
        friend class FrameBlackbox;

    public:
        RenderTexture(std::shared_ptr<Image> image,
            vk::Filter filter,
            vk::SamplerAddressMode tiling);

    private:
        void SetImage(std::shared_ptr<Image> image);
        void SetSampler(vk::Filter, vk::SamplerAddressMode tiling);

        ReallocationMask GetAllocationMask(uint32_t width,
            uint32_t height,
            VkFormat format,
            vk::Filter filter,
            vk::SamplerAddressMode tiling);

        std::unique_ptr<ISyncBarrier> GetBarrier(const ResourceAction action) override;

        ImageSyncState* GetSyncState()
        {
            return static_cast<ImageSyncState*>(syncState.get());
        }
    };
} // namespace Beer::Rendering
