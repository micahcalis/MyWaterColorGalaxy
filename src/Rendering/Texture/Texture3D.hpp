#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include <memory>

namespace Beer::Rendering
{
    class Texture3D : public ITexture
    {
    private:
        inline static std::shared_ptr<Texture3D> defaultBlack;

    public:
        static void SetFallbackTexture(std::shared_ptr<Image> defaultBlack)
        {
            Texture3D::defaultBlack = std::make_shared<Texture3D>(defaultBlack);
        }

        static void ResetFallbackTexture()
        {
            defaultBlack.reset();
        }

        static std::shared_ptr<Texture3D> GetFallbackTexture() { return defaultBlack; }

        static Texture3D Make(TextureMakeSettings settings,
            ComputeContext* context,
            std::shared_ptr<Sampler> sampler = Sampler::Get())
        {
            std::shared_ptr<Image> image = Image::Generate3D(settings.Width,
                settings.Height,
                settings.Depth,
                settings.Format,
                context,
                settings.GetThreads(),
                settings.KernelIndex);

            return Texture3D(image, sampler);
        }

        Texture3D(std::shared_ptr<Image> image,
            std::shared_ptr<Sampler> sampler = Sampler::Get())
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

        uint32_t GetDepth() const
        {
            return image->GetExtent().depth;
        }

        VkFormat GetFormat() const
        {
            return image->GetFormat();
        }
    };
} // namespace Beer::Rendering
