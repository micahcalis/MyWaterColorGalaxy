#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "DescriptorAllocator.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Texture/ITexture.hpp"

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

        UniformDescriptor(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
        vk::DescriptorSet GetSet(uint32_t frameIndex) const { return *descriptorSets[frameIndex]; }
        vk::DescriptorSetLayout GetLayout() const { return *layout; }
        void UpdateBufferInfo(uint32_t frameIndex, uint32_t binding, const Rendering::Buffer& buffer, size_t size);
        void UpdateImageInfo(uint32_t frameIndex, uint32_t binding, const Rendering::ITexture* texture);
    };
} // namespace Beer::Rendering
