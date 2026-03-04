#pragma once

#include "Rendering/Buffer/Image.hpp"
#include "vulkan/vulkan.hpp"
#include "Rendering/Sampler/Sampler.hpp"
#include <memory>

namespace Beer::Rendering
{
    class ITexture
    {
    protected:
        std::shared_ptr<Image> image;
        std::shared_ptr<Sampler> sampler;

    public:
        ITexture(std::shared_ptr<Image> image, std::shared_ptr<Sampler> sampler)
            : image(std::move(image)), sampler(std::move(sampler))
        {
        }

        virtual ~ITexture() = default;

        virtual const Sampler* GetSampler() const
        {
            return sampler.get();
        }

        virtual vk::DescriptorImageInfo GetDescriptorInfo()
        {
            return vk::DescriptorImageInfo(
                sampler->GetVk(),
                image->GetDefaultView(),
                vk::ImageLayout::eShaderReadOnlyOptimal);
        }

        virtual VkImageView const GetImageView() const
        {
            return image->GetDefaultView();
        }

        virtual const vk::Extent3D GetExtent() const
        {
            return image->GetExtent();
        }
    };
} // namespace Beer::Rendering
