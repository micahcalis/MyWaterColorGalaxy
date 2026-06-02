#include "Core/Application/Managers/FontAssetManager.hpp"
#include "Core/Application/Jobs/ImageUploadJob.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Assets/FontAssetLoader.hpp"
#include "Core/Assets/ImageAsset.hpp"
#include "Core/Assets/ImageLoader.hpp"
#include "Rendering/Text/FontAsset.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include <memory>

namespace Beer::Core
{
    constexpr const uint32_t COLOR_CHANNELS = 4;

    std::shared_ptr<Rendering::FontAsset> FontAssetManager::Load(const std::filesystem::path& path)
    {
        std::filesystem::path jsonPath = AssetUtilities::GetFontAssetJsonPath(path);
        auto characterMap = FontAssetLoader::ReadCharacterMap(jsonPath);

        ImageAsset atlasAsset = ImageLoader::LoadImage(path, COLOR_CHANNELS);

        std::shared_ptr<Rendering::Image> image = std::make_shared<Rendering::Image>(
            Rendering::Image::CreateImage2D(atlasAsset.Width,
                atlasAsset.Height,
                VK_FORMAT_R8G8B8A8_UNORM,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
                vk::ImageAspectFlagBits::eColor,
                1,
                false,
                *device));

        std::unique_ptr<ImageUploadJob> uploadJob = std::make_unique<ImageUploadJob>(
            image, atlasAsset);

        uploadManager->AddJob(std::move(uploadJob));

        std::shared_ptr<Rendering::Texture2D> atlasTexture = std::make_shared<Rendering::Texture2D>(image);

        return std::make_shared<Rendering::FontAsset>(std::move(atlasTexture),
            std::move(characterMap));
    }

    const std::filesystem::path FontAssetManager::GetPath(const std::string& name)
    {
        return AssetUtilities::GetFontAssetPath(name);
    }
} // namespace Beer::Core
