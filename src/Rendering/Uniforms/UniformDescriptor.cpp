#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "DescriptorAllocator.hpp"
#include "Rendering/Texture/ITexture.hpp"

namespace Beer::Rendering
{
    UniformDescriptor::UniformDescriptor(const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
        layout = vk::raii::DescriptorSetLayout(descriptorAllocator->Device->GetLogicalDevice(),
            layoutInfo);

        for (int i = 0; i < descriptorAllocator->FramesInFlight; i++)
        {
            descriptorSets.emplace_back(descriptorAllocator->Allocate(*layout));
        }
    }

    void UniformDescriptor::UpdateBufferInfo(uint32_t frameIndex,
        uint32_t binding,
        const Rendering::Buffer& buffer,
        size_t size)
    {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer.GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = size;

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = *descriptorSets[frameIndex];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        descriptorAllocator->Device->GetLogicalDevice().updateDescriptorSets(descriptorWrite, nullptr);
    }

    void UniformDescriptor::UpdateImageInfo(uint32_t frameIndex, uint32_t binding, const Rendering::ITexture* texture)
    {
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetSampler()->GetVk();
        imageInfo.imageView = texture->GetImageView();
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = *descriptorSets[frameIndex];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        descriptorAllocator->Device->GetLogicalDevice().updateDescriptorSets(descriptorWrite, nullptr);
    }
} // namespace Beer::Rendering
