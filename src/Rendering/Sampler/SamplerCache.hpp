#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <unordered_map>
#include "Rendering/Sampler/SamplerKey.hpp"
#include "Core/Application/Renderer/Device.hpp"

namespace Beer::Rendering
{
    class SamplerCache
    {
    private:
        const Core::Device& device;
        std::unordered_map<SamplerKey, vk::raii::Sampler> cache;

    public:
        SamplerCache(const Core::Device& device);
        const vk::raii::Sampler& GetSampler(const SamplerKey& key);

    private:
        vk::raii::Sampler CreateSampler(const SamplerKey& key);
    };
} // namespace Beer::Rendering
