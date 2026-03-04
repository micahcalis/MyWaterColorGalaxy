#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <unordered_map>
#include "Rendering/Sampler/SamplerKey.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Sampler.hpp"

namespace Beer::Rendering
{
    class SamplerCache
    {
    private:
        const Core::Device& device;
        std::unordered_map<SamplerKey, std::shared_ptr<Sampler>> cache;

    public:
        SamplerCache(const Core::Device& device);
        std::shared_ptr<Sampler> GetSampler(const SamplerKey& key);

    private:
        vk::raii::Sampler CreateSampler(const SamplerKey& key);
    };
} // namespace Beer::Rendering
