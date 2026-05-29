#pragma once

#include "Rendering/Sampler/Sampler.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.h>
#include "Rendering/Texture/TextureMakeSettings.hpp"

namespace Beer::Rendering
{
    class Texture2D : public ITexture
    {
    private:
        inline static std::shared_ptr<Texture2D> defaultBlack;

    public:
        static void SetFallbackTexture(std::shared_ptr<Image> defaultBlack)
        {
            Texture2D::defaultBlack = std::make_shared<Texture2D>(defaultBlack);
        }

        static void ResetFallbackTexture()
        {
            defaultBlack.reset();
        }

        static std::shared_ptr<Texture2D> GetFallbackTexture() { return defaultBlack; }

        static Texture2D Make(TextureMakeSettings settings,
            ComputeContext* context,
            std::shared_ptr<Sampler> sampler = Sampler::Get())
        {
            std::shared_ptr<Image> image = Image::Generate2D(settings.Width,
                settings.Height,
                settings.Format,
                settings.LayerCount,
                context,
                settings.GetThreads(),
                settings.KernelIndex);

            return Texture2D(image, sampler);
        }

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
