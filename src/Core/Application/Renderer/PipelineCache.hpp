#pragma once

#include "PipelineData.hpp"
#include "PipelineKey.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <unordered_map>
#include "Core/Application/Renderer/PipelineKey.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"

namespace Beer::Core
{
    class PipelineCache
    {
    private:
        const vk::raii::Device& logicalDevice;
        const Swapchain& swapchain;
        vk::DescriptorSetLayout globalSetLayout;
        std::unordered_map<PipelineKey, vk::raii::Pipeline> cache;
        std::vector<vk::raii::PipelineLayout> cachedLayouts;
        const vk::Format depthFormat;

    public:
        PipelineCache(const vk::raii::Device& device, const Swapchain& swapchain, vk::DescriptorSetLayout globalSetLayout, const vk::Format depthFormat);
        const vk::raii::Pipeline& GetPipeline(const PipelineKey& key, const PipelineData& data);

    private:
        vk::raii::Pipeline CreatePipeline(const PipelineData& data);
    };
} // namespace Beer::Core
