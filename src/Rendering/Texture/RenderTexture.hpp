#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncState.hpp"
#include "Rendering/Texture/ReallocationFlags.hpp"
#include "Rendering/Pipeline/Frame//Resource/IRenderResource.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    enum class TextureAccess
    {
        Standard,
        ReadWrite
    };
}

namespace Beer::Rendering
{
    class RenderTexture : public ITexture
        , public IRenderResource
    {
        friend class FrameBlackbox;
        friend class FrameGraph;

    private:
        std::string name;

    public:
        RenderTexture(const std::string& name,
            std::shared_ptr<Image> image,
            vk::Filter filter,
            vk::SamplerAddressMode tiling);

        uint32_t Width() const { return image->GetData().Extent.width; }
        uint32_t Height() const { return image->GetData().Extent.height; }
        std::string Name() const { return name; }
        [[nodiscard]] Image* GetImage() const { return image.get(); }
        std::unique_ptr<ISyncBarrier> GetBarrier(const ResourceAction action) override;
        ResourceAction GetCurrentAction() const { return syncState->CurrentAction; }

    private:
        void SetImage(std::shared_ptr<Image> image);
        void SetSampler(vk::Filter, vk::SamplerAddressMode tiling);

        ReallocationMask GetAllocationMask(uint32_t width,
            uint32_t height,
            VkFormat format,
            vk::Filter filter,
            vk::SamplerAddressMode tiling,
            uint32_t layerCount);

        ImageSyncState* GetSyncState()
        {
            return static_cast<ImageSyncState*>(syncState.get());
        }

        vk::RenderingAttachmentInfo GetAttachmentInfo(const ResetOperator& resetOperator,
            bool& isDepth);
    };
} // namespace Beer::Rendering
