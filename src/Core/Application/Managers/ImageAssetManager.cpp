#include "Core/Application/Managers/ImageAssetManager.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Assets/ImageAsset.hpp"
#include "Core/Assets/ImageLoader.hpp"
#include "Core/Application/Jobs/ImageUploadJob.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include <glm/glm.hpp>

namespace Beer::Core
{
    constexpr const uint32_t COLOR_CHANNELS = 4;
    static uint32_t WHITE_PIXEL = 0xFFFFFFFF;
    constexpr const vk::DeviceSize PIXEL_SIZE = vk::DeviceSize(sizeof(WHITE_PIXEL));

    std::shared_ptr<Rendering::Image> ImageAssetManager::Load(const std::filesystem::path& path)
    {
        ImageAsset imageAsset = ImageLoader::LoadImage(path, COLOR_CHANNELS);

        std::shared_ptr<Rendering::Image> image = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(imageAsset.Width,
                imageAsset.Height,
                VK_FORMAT_R8G8B8A8_SRGB,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
                vk::ImageAspectFlagBits::eColor,
                *device));

        std::unique_ptr<ImageUploadJob> uploadJob = std::make_unique<ImageUploadJob>(
            image, imageAsset);

        uploadManager->AddJob(std::move(uploadJob));

        return image;
    }

    const std::filesystem::path ImageAssetManager::GetPath(const std::string& name)
    {
        return AssetUtilities::GetTexturePath(name);
    }

    void ImageAssetManager::InitializeTextureFallback()
    {
        std::shared_ptr<Rendering::Image> image = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(1,
                1,
                VK_FORMAT_R8G8B8A8_SRGB,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
                vk::ImageAspectFlagBits::eColor,
                *device));

        std::unique_ptr<ImageUploadJob> uploadJob = std::make_unique<ImageUploadJob>(
            image, (void*)&WHITE_PIXEL, PIXEL_SIZE);

        uploadManager->AddJob(std::move(uploadJob));

        Rendering::Texture2D::SetFallbackTexture(image);
    }
} // namespace Beer::Core
