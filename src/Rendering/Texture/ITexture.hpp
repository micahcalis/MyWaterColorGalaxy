#pragma once

#include "Rendering/Buffer/Image.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    class ITexture
    {
    protected:
        std::shared_ptr<Image> image;
        vk::Sampler sampler;

    public:
        ITexture(std::shared_ptr<Image> image, vk::Sampler sampler)
            : image(std::move(image)), sampler(sampler)
        {
        }

        virtual ~ITexture() = default;

        virtual vk::Sampler GetSampler()
        {
            return sampler;
        }

        virtual vk::DescriptorImageInfo GetDescriptorInfo()
        {
            return vk::DescriptorImageInfo(
                sampler,
                image->GetDefaultView(),
                vk::ImageLayout::eShaderReadOnlyOptimal);
        }

        virtual VkImageView GetImageView()
        {
            return image->GetDefaultView();
        }

        virtual vk::Extent3D GetExtent()
        {
            return image->GetExtent();
        }
    };
} // namespace Beer::Rendering
