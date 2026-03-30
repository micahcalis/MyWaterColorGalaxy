#include "Rendering/Shader/Globals/GlobalBuffer.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    GlobalBuffer::GlobalBuffer(std::vector<BufferBinding> bufferBindings)
        : bufferBindings(std::move(bufferBindings))
    {
        auto bindings = GetBindings();
        minAlignment = UniformDescriptor::GetMinAlignment();

        descriptor = std::make_unique<UniformDescriptor>(bindings);
        size_t totalSize = GetTotalSize();

        for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
        {
            buffers.push_back(std::make_shared<Buffer>(
                Buffer::CreateUniform(totalSize)));

            size_t currentOffset = 0;

            for (const auto& bufferData : this->bufferBindings)
            {
                descriptor->UpdateBufferInfo(
                    i,
                    bufferData.Binding,
                    *buffers[i],
                    bufferData.Size,
                    currentOffset);

                currentOffset += (bufferData.Size + minAlignment - 1) & ~(minAlignment - 1);
            }
        }
    }

    void GlobalBuffer::Update(uint32_t binding, const void* data)
    {
        size_t currentOffset = 0;
        size_t targetSize = 0;
        bool found = false;

        for (const auto& bufferData : bufferBindings)
        {
            if (bufferData.Binding == binding)
            {
                targetSize = bufferData.Size;
                found = true;
                break;
            }

            currentOffset += (bufferData.Size + minAlignment - 1) & ~(minAlignment - 1);
        }

        if (!found)
        {
            return;
        }

        buffers[UniformDescriptor::GetFrameIndex()]->Upload(data, targetSize, currentOffset);
    }

    std::vector<vk::DescriptorSetLayoutBinding> GlobalBuffer::GetBindings()
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        bindings.reserve(bufferBindings.size());

        for (const auto& bufferData : bufferBindings)
        {
            bindings.emplace_back(
                bufferData.Binding,
                vk::DescriptorType::eUniformBuffer,
                bufferData.DescriptorCount,
                vk::ShaderStageFlagBits::eAllGraphics | vk::ShaderStageFlagBits::eCompute);
        }

        return bindings;
    }

    size_t GlobalBuffer::GetTotalSize()
    {
        size_t totalSize = 0;

        for (const auto& buffer : bufferBindings)
        {
            totalSize += (buffer.Size + minAlignment - 1) & ~(minAlignment - 1);
        }

        return totalSize;
    }
} // namespace Beer::Rendering
