#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "DescriptorAllocator.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "vulkan/vulkan.hpp"
#include <stdexcept>

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

    UniformDescriptor::~UniformDescriptor()
    {
        if (!descriptorSets.empty())
        {
            deletionQueues[frameIndex].push_back({std::move(layout),
                std::move(descriptorSets)});
        }
    }

    void UniformDescriptor::UpdateBufferInfo(uint32_t frameIndex,
        uint32_t binding,
        const Rendering::Buffer& buffer,
        size_t size,
        size_t offset)
    {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer.GetHandle();
        bufferInfo.offset = offset;
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

    void UniformDescriptor::UpdateImageInfo(uint32_t frameIndex, const ShaderProperty* property, const Rendering::ITexture* texture)
    {
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageView = texture->GetImageView();
        if (property->Type == PropertyType::RWTexture2D || property->Type == PropertyType::RWTexture2DArray)
        {
            imageInfo.imageLayout = vk::ImageLayout::eGeneral;
            imageInfo.sampler = nullptr;
        } else if (property->Type == PropertyType::Texture2D || property->Type == PropertyType::Texture2DArray)
        {
            imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfo.sampler = texture->GetSampler()->GetVk();
        } else
        {
            throw std::runtime_error("Unsupported Texture Type tried binding to Material Properties");
        }

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = *descriptorSets[frameIndex];
        descriptorWrite.dstBinding = property->Binding;
        descriptorWrite.dstArrayElement = 0;

        bool isStorageImage = (property->Type == PropertyType::RWTexture2D || property->Type == PropertyType::RWTexture2DArray);
        descriptorWrite.descriptorType = isStorageImage
            ? vk::DescriptorType::eStorageImage
            : vk::DescriptorType::eCombinedImageSampler;

        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        descriptorAllocator->Device->GetLogicalDevice().updateDescriptorSets(descriptorWrite, nullptr);
    }

    void UniformDescriptor::UpdateImageInfo(uint32_t frameIndex, uint32_t binding, TextureAccess access, const ITexture* texture)
    {
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageView = texture->GetImageView();
        if (access == TextureAccess::ReadWrite)
        {
            imageInfo.imageLayout = vk::ImageLayout::eGeneral;
            imageInfo.sampler = nullptr;
        } else if (access == TextureAccess::Standard)
        {
            imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfo.sampler = texture->GetSampler()->GetVk();
        } else
        {
            throw std::runtime_error("Unsupported Texture Access Type tried binding to Uniform Descriptor");
        }

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = *descriptorSets[frameIndex];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = access == TextureAccess::ReadWrite
            ? vk::DescriptorType::eStorageImage
            : vk::DescriptorType::eCombinedImageSampler;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        descriptorAllocator->Device->GetLogicalDevice().updateDescriptorSets(descriptorWrite, nullptr);
    }

    void UniformDescriptor::UpdateStructuredBufferInfo(uint32_t frameIndex, const uint32_t binding, const PhaseBuffer* buffer)
    {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer->GetHandle()->GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = buffer->Size();

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = *descriptorSets[frameIndex];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eStorageBuffer;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        descriptorAllocator->Device->GetLogicalDevice().updateDescriptorSets(descriptorWrite, nullptr);
    }
} // namespace Beer::Rendering
