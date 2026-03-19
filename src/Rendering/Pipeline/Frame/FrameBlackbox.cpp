#include "Rendering/Pipeline/Frame/FrameBlackbox.hpp"
#include "Core/Application/Jobs/ImageClearJob.hpp"
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
            throw std::runtime_error(std::format("Blackbox already has Render Texture named: {}", name));
        }

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

        blackbox[name] = std::make_unique<RenderTexture>(image,
            filter,
            tiling);

        return GetResource<RenderTexture>(name);
    }
} // namespace Beer::Rendering
