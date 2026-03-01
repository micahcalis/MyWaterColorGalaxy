#include "Core/Assets/ImageLoader.hpp"
#include "ImageAsset.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/stb/stb_image.h>
#include <filesystem>
#include "Core/Application/Utilities/AssetUtilities.hpp"

namespace Beer::Core
{
    ImageAsset ImageLoader::LoadImage(const std::filesystem::path& path, int desiredChannels)
    {
        ImageAsset imageAsset;
        int texWidth, texHeight, fileChannels;

        imageAsset.Pixels = stbi_load(path.string().c_str(),
            &texWidth,
            &texHeight,
            &fileChannels,
            desiredChannels);

        if (!imageAsset.Pixels)
        {
            throw std::runtime_error("failed to load texture image");
        }

        imageAsset.Width = static_cast<uint32_t>(texWidth);
        imageAsset.Height = static_cast<uint32_t>(texHeight);

        imageAsset.Channels = (desiredChannels == STBI_default) ? fileChannels : desiredChannels;
        imageAsset.Size = imageAsset.Height * imageAsset.Width * imageAsset.Channels;

        return imageAsset;
    }

    void ImageLoader::FreeImage(const ImageAsset& imageAsset)
    {
        stbi_image_free(imageAsset.Pixels);
    }
} // namespace Beer::Core
