#include "Rendering/Sampler/SamplerCache.hpp"
#include <math.h>
#include <memory>

namespace Beer::Rendering
{
    SamplerCache::SamplerCache(const Core::Device& device)
        : device(device)
    {
    }

    std::shared_ptr<Sampler> SamplerCache::GetSampler(const SamplerKey& key)
    {
        auto lookUp = cache.find(key);

        if (lookUp != cache.end())
        {
            return lookUp->second;
        }

        std::shared_ptr<Sampler> sampler = std::make_shared<Sampler>(
            CreateSampler(key),
            key.Filter,
            key.Tiling,
            key.MaxAnisotropy);

        auto result = cache.emplace(key, sampler);

        return result.first->second;
    }

    vk::raii::Sampler SamplerCache::CreateSampler(const SamplerKey& key)
    {
        vk::PhysicalDeviceProperties properties = device.GetPhysicalDevice().getProperties();

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = key.Filter;
        samplerInfo.minFilter = key.Filter;
        samplerInfo.addressModeU = key.Tiling;
        samplerInfo.addressModeV = key.Tiling;
        samplerInfo.addressModeW = key.Tiling;
        samplerInfo.anisotropyEnable = vk::True;
        samplerInfo.maxAnisotropy = std::min(properties.limits.maxSamplerAnisotropy, key.MaxAnisotropy);
        samplerInfo.compareEnable = vk::False;
        samplerInfo.compareOp = vk::CompareOp::eAlways;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = vk::False;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = vk::LodClampNone;

        return vk::raii::Sampler(device.GetLogicalDevice(), samplerInfo);
    }

} // namespace Beer::Rendering
