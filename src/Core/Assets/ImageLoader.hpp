#pragma once

#include "Core/Assets/ImageAsset.hpp"
#include <string>

namespace Beer::Core
{
    class ImageLoader
    {
    public:
        static ImageAsset LoadImage(const std::string& name, int desiredChannels);
        static void FreeImage(const ImageAsset& imageAsset);
    };
} // namespace Beer::Core
