#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "DescriptorAllocator.hpp"
#include "Rendering/Buffer/Buffer.hpp"

namespace Beer::Rendering
{
    class UniformDescriptor
    {
    private:
        inline static DescriptorAllocator* descriptorAllocator = nullptr;

        vk::raii::DescriptorSetLayout layout = nullptr;
        std::vector<vk::raii::DescriptorSet> descriptorSets;

    public:
        static void SetDescriptorAllocator(DescriptorAllocator* descriptorAllocator)
        {
            UniformDescriptor::descriptorAllocator = descriptorAllocator;
        }

        vk::DescriptorSet GetSet(uint32_t frameIndex) const { return *descriptorSets[frameIndex]; }
        vk::DescriptorSetLayout GetLayout() const { return *layout; }

        UniformDescriptor(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);

        void UpdateBufferInfo(uint32_t frameIndex, uint32_t binding, const Rendering::Buffer& buffer, size_t size);
    };
} // namespace Beer::Rendering
