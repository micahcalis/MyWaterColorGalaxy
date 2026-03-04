#pragma once

#include "Core/Application/Renderer/Device.hpp"
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Rendering
{
    class DescriptorAllocator
    {
    public:
        const uint32_t FramesInFlight;
        const Core::Device* Device;

    private:
        std::vector<vk::raii::DescriptorPool> currentPools;
        int currentPoolIndex = -1;

    public:
        DescriptorAllocator(const uint32_t framesInFlight, const Core::Device* device);
        vk::raii::DescriptorSet Allocate(vk::DescriptorSetLayout layout);

    private:
        void CreatePool();
        vk::DescriptorPool GetCurrentPool() { return *currentPools[currentPoolIndex]; }
    };
} // namespace Beer::Rendering
