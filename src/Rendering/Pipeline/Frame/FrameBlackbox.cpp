#include "Rendering/Pipeline/Frame/FrameBlackbox.hpp"
#include "Core/Application/Jobs/ImageClearJob.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "FrameBlackbox.hpp"
#include "Rendering/Texture/ReallocationFlags.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    constexpr VkImageUsageFlags COLOR_TEX_FLAGS = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT
        | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT
        | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    constexpr VkImageUsageFlags DEPTH_TEX_FLAGS = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
        | VK_IMAGE_USAGE_SAMPLED_BIT
        | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
        | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    RenderTexture* FrameBlackbox::CreateRenderTexture2D(const std::string& name,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        vk::Filter filter,
        vk::SamplerAddressMode tiling,
        glm::vec4 clearColor)
    {
        if (blackbox.contains(name))
        {
            return static_cast<RenderTexture*>(blackbox[name].get());
        }

        std::shared_ptr<Image> image = CreateRenderTextureImage(width,
            height,
            format,
            clearColor);

        blackbox[name] = std::make_unique<RenderTexture>(name,
            std::move(image),
            filter,
            tiling);

        return GetResource<RenderTexture>(name);
    }

    RenderTexture* FrameBlackbox::ReallocateIfNeeded(const std::string& name,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        vk::Filter filter,
        vk::SamplerAddressMode tiling,
        glm::vec4 clearColor)
    {
        RenderTexture* renderTexture = GetResource<RenderTexture>(name);

        if (renderTexture == nullptr)
        {
            renderTexture = CreateRenderTexture2D(name, width, height, format, filter, tiling, clearColor);
            return renderTexture;
        }

        ReallocationMask mask = renderTexture->GetAllocationMask(width, height, format, filter, tiling);

        if (mask.Has(ReallocationFlag::Image))
        {
            std::shared_ptr<Image> image = CreateRenderTextureImage(width,
                height,
                format,
                clearColor);

            renderTexture->SetImage(std::move(image));
        }

        if (mask.Has(ReallocationFlag::Sampler))
        {
            renderTexture->SetSampler(filter, tiling);
        }

        return renderTexture;
    }

    std::shared_ptr<Image> FrameBlackbox::CreateRenderTextureImage(uint32_t width,
        uint32_t height,
        VkFormat format,
        glm::vec4 clearColor)
    {
        bool isDepth = Core::ImageUtilities::IsDepthFormat(static_cast<vk::Format>(format));
        VkImageUsageFlags usageFlags = isDepth ? DEPTH_TEX_FLAGS : COLOR_TEX_FLAGS;
        vk::ImageAspectFlagBits aspectFlags = isDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;

        return std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(width,
                height,
                format,
                usageFlags,
                aspectFlags,
                *device));
    }
} // namespace Beer::Rendering
