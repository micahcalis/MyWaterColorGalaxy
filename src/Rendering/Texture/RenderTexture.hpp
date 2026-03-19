#pragma once

#include "Rendering/Pipeline/Frame//Resource/IRenderResource.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class RenderTexture : public ITexture
        , public IRenderResource
    {
    public:
        RenderTexture(std::shared_ptr<Image> image,
            vk::Filter filter,
            vk::SamplerAddressMode tiling);
    };
} // namespace Beer::Rendering
