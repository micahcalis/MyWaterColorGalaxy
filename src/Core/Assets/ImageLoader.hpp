#pragma once

#include "Core/Assets/ImageAsset.hpp"
#include <filesystem>

namespace Beer::Core
{
    class ImageLoader
    {
    public:
        static ImageAsset LoadImage(const std::filesystem::path& path, int desiredChannels);
        static void FreeImage(const ImageAsset& imageAsset);
    };
} // namespace Beer::Core
