#pragma once

#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct SamplerData
    {
        vk::Filter Filter;
        vk::SamplerAddressMode Tiling;
        float MaxAnisotropy;

        SamplerData(vk::Filter filter,
            vk::SamplerAddressMode tiling,
            float maxAnisotropy)
            : Filter(filter), Tiling(tiling), MaxAnisotropy(maxAnisotropy)
        {
        }
    };
} // namespace Beer::Rendering
