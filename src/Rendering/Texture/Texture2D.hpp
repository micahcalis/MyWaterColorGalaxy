#pragma once

#include "Rendering/Sampler/Sampler.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.h>

namespace Beer::Rendering
{
    class Texture2D : public ITexture
    {
    public:
        Texture2D(std::shared_ptr<Image> image,
            std::shared_ptr<Sampler> sampler = Sampler::Get())
            : ITexture(image, sampler)
        {
        }

        Texture2D(const std::string& imageName,
            std::shared_ptr<Sampler> sampler = Sampler::Get())
            : ITexture(Rendering::Image::GetAsset(imageName), sampler)
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
