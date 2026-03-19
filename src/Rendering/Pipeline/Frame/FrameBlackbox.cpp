#include "Rendering/Pipeline/Frame/FrameBlackbox.hpp"
#include "Core/Application/Jobs/ImageClearJob.hpp"
#include "FrameBlackbox.hpp"
#include "Rendering/Texture/ReallocationFlags.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    constexpr VkImageUsageFlags RENDER_TEX_FLAGS = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT
        | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT
        | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

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

        blackbox[name] = std::make_unique<RenderTexture>(std::move(image),
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
        std::shared_ptr<Rendering::Image> image = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(width,
                height,
                format,
                RENDER_TEX_FLAGS,
                vk::ImageAspectFlagBits::eColor,
                *device));

        std::unique_ptr<Core::ImageClearJob> clearJob = std::make_unique<Core::ImageClearJob>(
            image, clearColor);

        uploadManager->AddJob(std::move(clearJob));

        return image;
    }
} // namespace Beer::Rendering
