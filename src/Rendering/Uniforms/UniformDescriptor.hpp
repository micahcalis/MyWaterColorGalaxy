#pragma once

#include <cstdint>
#include <vulkan/vulkan_raii.hpp>
#include "DescriptorAllocator.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Texture/ITexture.hpp"

namespace Beer::Rendering
{
    class UniformDescriptor
    {
    private:
        inline static DescriptorAllocator* descriptorAllocator = nullptr;
        inline static uint32_t frameIndex = 0;

        vk::raii::DescriptorSetLayout layout = nullptr;
        std::vector<vk::raii::DescriptorSet> descriptorSets;

    public:
        static void SetDescriptorAllocator(DescriptorAllocator* descriptorAllocator)
        {
            UniformDescriptor::descriptorAllocator = descriptorAllocator;
        }

        static void SetFrameIndex(uint32_t frameIndex)
        {
            UniformDescriptor::frameIndex = frameIndex;
        }

        static size_t GetMinAlignment()
        {
            return descriptorAllocator->Device->GetMinUniformBufferOffset();
        }

        UniformDescriptor(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
        vk::DescriptorSet GetSet(uint32_t frameIndex) const { return *descriptorSets[frameIndex]; }
        vk::DescriptorSetLayout GetLayout() const { return *layout; }
        void UpdateBufferInfo(uint32_t frameIndex, uint32_t binding, const Rendering::Buffer& buffer, size_t size, size_t offset = 0);
        void UpdateImageInfo(uint32_t frameIndex, const ShaderProperty* property, const Rendering::ITexture* texture);

        static uint32_t GetFramesInFlight() { return descriptorAllocator->FramesInFlight; }
        static uint32_t GetFrameIndex() { return frameIndex; }
    };
} // namespace Beer::Rendering
