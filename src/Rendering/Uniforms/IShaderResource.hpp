#pragma once

#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class IShaderResource
    {
    protected:
        std::unique_ptr<UniformDescriptor> descriptor = nullptr;
        std::vector<std::shared_ptr<Buffer>> buffers;

    public:
        virtual ~IShaderResource() = default;

        UniformDescriptor* GetDescriptor() { return descriptor.get(); }

        virtual void Bind(vk::raii::CommandBuffer& cmd,
            vk::PipelineLayout pipelineLayout,
            uint32_t setIndex,
            uint32_t frameIndex) const
        {
            if (descriptor)
            {
                cmd.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    pipelineLayout,
                    setIndex,
                    descriptor->GetSet(frameIndex),
                    nullptr);
            }
        }

    protected:
        virtual std::vector<vk::DescriptorSetLayoutBinding> GetBindings() = 0;
    };
} // namespace Beer::Rendering
