#include "Rendering/Shader/Globals/EngineGlobalBuffer.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    EngineGlobalBuffer::EngineGlobalBuffer()
    {
        auto bindings = GetBindings();

        descriptor = std::make_unique<UniformDescriptor>(bindings);

        for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
        {
            buffers.push_back(std::make_shared<Buffer>(
                Buffer::CreateUniform(sizeof(EngineGlobals))));

            descriptor->UpdateBufferInfo(i,
                EngineGlobals::BINDING,
                *buffers[i],
                sizeof(EngineGlobals));
        }
    }

    void EngineGlobalBuffer::Update(const EngineGlobals& data)
    {
        buffers[UniformDescriptor::GetFrameIndex()]->Upload(&data, sizeof(EngineGlobals));
    }

    std::vector<vk::DescriptorSetLayoutBinding> EngineGlobalBuffer::GetBindings()
    {
        return {vk::DescriptorSetLayoutBinding(EngineGlobals::BINDING,
            vk::DescriptorType::eUniformBuffer,
            EngineGlobals::DESC_COUNT,
            vk::ShaderStageFlagBits::eAllGraphics)};
    }
} // namespace Beer::Rendering
