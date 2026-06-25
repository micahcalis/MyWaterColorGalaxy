#include "Core/Application/Managers/ImageAssetManager.hpp"
#include "Core/Application/Jobs/ImageCopyJob.hpp"
#include "Core/Application/Jobs/ImageGenerationJob.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Assets/ImageAsset.hpp"
#include "Core/Assets/ImageLoader.hpp"
#include "Core/Application/Jobs/ImageUploadJob.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace Beer::Core
{
    constexpr const uint32_t COLOR_CHANNELS = 4;
    static uint32_t WHITE_PIXEL = 0xFFFFFFFF;

    static std::array<uint32_t, 6> WHITE_CUBEMAP = {
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

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
                1,
                false,
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

    void ImageAssetManager::InitializeTextureFallbacks()
    {
        std::shared_ptr<Rendering::Image> image2D = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(1,
                1,
                VK_FORMAT_R8G8B8A8_UNORM,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT
                    | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT
                    | VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT,
                vk::ImageAspectFlagBits::eColor,
                1,
                false,
                *device));

        std::unique_ptr<ImageUploadJob> uploadJob2D = std::make_unique<ImageUploadJob>(
            image2D, (void*)&WHITE_PIXEL, PIXEL_SIZE);

        uploadManager->AddJob(std::move(uploadJob2D));

        std::shared_ptr<Rendering::Image> imageCube = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(1,
                1,
                VK_FORMAT_R8G8B8A8_UNORM,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT
                    | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT
                    | VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT,
                vk::ImageAspectFlagBits::eColor,
                6,
                true,
                *device));

        std::unique_ptr<ImageUploadJob> uploadJobCube = std::make_unique<ImageUploadJob>(
            imageCube, (void*)&WHITE_CUBEMAP, sizeof(WHITE_CUBEMAP));

        uploadManager->AddJob(std::move(uploadJobCube));

        Rendering::Texture2D::SetFallbackTextures(image2D, imageCube);

        std::shared_ptr<Rendering::Image> image3D = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage3D(1,
                1,
                1,
                VK_FORMAT_R8G8B8A8_UNORM,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT
                    | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT
                    | VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT,
                vk::ImageAspectFlagBits::eColor,
                *device));

        std::unique_ptr<ImageUploadJob> uploadJob3D = std::make_unique<ImageUploadJob>(
            image3D, (void*)&WHITE_PIXEL, PIXEL_SIZE);

        uploadManager->AddJob(std::move(uploadJob3D));

        Rendering::Texture3D::SetFallbackTexture(image3D);
    }

    std::shared_ptr<Rendering::Image> ImageAssetManager::CreateEmpty2D(uint32_t width,
        uint32_t height,
        VkFormat format,
        uint32_t layerCount,
        bool isCubemap)
    {
        return std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(width,
                height,
                format,
                VK_IMAGE_USAGE_SAMPLED_BIT
                    | VK_IMAGE_USAGE_STORAGE_BIT
                    | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                    | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                vk::ImageAspectFlagBits::eColor,
                layerCount,
                isCubemap,
                *device));
    }

    std::shared_ptr<Rendering::Image> ImageAssetManager::CreateEmpty3D(uint32_t width,
        uint32_t height,
        uint32_t depth,
        VkFormat format)
    {
        return std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage3D(width,
                height,
                depth,
                format,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT
                    | VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT,
                vk::ImageAspectFlagBits::eColor,
                *device));
    }

    void ImageAssetManager::GenerateFromEmpty(std::shared_ptr<Rendering::Image> image,
        Rendering::ComputeContext* context,
        Rendering::Threads threads,
        uint32_t kernelIndex)
    {
        std::unique_ptr<Core::ImageGenerationJob> imageGenerationJob = std::make_unique<Core::ImageGenerationJob>(
            image,
            context,
            threads,
            kernelIndex);

        uploadManager->AddJob(std::move(imageGenerationJob));
    }

    void ImageAssetManager::CopyImage(std::shared_ptr<Rendering::Image> sourceImage,
        std::shared_ptr<Rendering::Image> destinationImage)
    {
        std::unique_ptr<Core::ImageCopyJob> imageCopyJob = std::make_unique<Core::ImageCopyJob>(
            sourceImage,
            destinationImage);

        uploadManager->AddJob(std::move(imageCopyJob));
    }
} // namespace Beer::Core
