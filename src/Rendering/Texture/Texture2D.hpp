#pragma once

#include "Core/Application/Jobs/ImageGenerationJob.hpp"
#include "Rendering/Compute/Threads.hpp"
#include "Rendering/Sampler/Sampler.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.h>

namespace Beer::Rendering
{
    struct TextureMakeSettings
    {
    public:
        uint32_t Width = 1;
        uint32_t Height = 1;
        VkFormat Format = VK_FORMAT_R8G8B8A8_UNORM;
        uint32_t LayerCount = 1;
        uint32_t KernelIndex = 0;
        uint32_t GroupSizeX = 8;
        uint32_t GroupSizeY = 8;

    public:
        Threads GetThreads()
        {
            return Threads::GetCustom(Width, GroupSizeX, Height, GroupSizeY);
        }
    };

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
            std::shared_ptr<Image> image = Image::Generate(settings.Width,
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
