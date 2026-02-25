#pragma once

#include "Rendering/Texture/ITexture.hpp"
#include <cstdint>
#include <vulkan/vulkan.h>

namespace Beer::Rendering
{
    class Texture2D : public ITexture
    {
    public:
        Texture2D(std::shared_ptr<Image> image, vk::Sampler sampler)
            : ITexture(image, sampler)
        {
        }

        uint32_t GetWidth() const
        {
            return image->GetExtent().width;
        }

        uint32_t GetHeight() const
        {
            return image->GetExtent().height;
        }

        VkFormat GetFormat() const
        {
            return image->GetFormat();
        }
    };
} // namespace Beer::Rendering
