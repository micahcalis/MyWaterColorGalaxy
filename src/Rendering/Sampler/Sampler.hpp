#pragma once

#include "SamplerData.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

namespace Beer::Rendering
{
    class SamplerCache;
}

namespace Beer::Rendering
{
    class Sampler
    {
    private:
        inline static SamplerCache* samplerCache = nullptr;

        vk::raii::Sampler sampler = nullptr;
        SamplerData data;

    public:
        static void SetSamplerCache(SamplerCache* samplerCache)
        {
            Sampler::samplerCache = samplerCache;
        }

        static std::shared_ptr<Sampler> Get(vk::Filter filter = vk::Filter::eLinear,
            vk::SamplerAddressMode tiling = vk::SamplerAddressMode::eRepeat,
            float maxAnisotropy = 10.0f);

        Sampler(vk::raii::Sampler sampler,
            vk::Filter filter,
            vk::SamplerAddressMode tiling,
            float maxAnisotropy)
            : sampler(std::move(sampler)), data(SamplerData(filter, tiling, maxAnisotropy))
        {
        }

        ~Sampler();

        vk::Sampler GetVk() const
        {
            return sampler;
        }

        const SamplerData GetData() const
        {
            return data;
        }
    };
} // namespace Beer::Rendering
