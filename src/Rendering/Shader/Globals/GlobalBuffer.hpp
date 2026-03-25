#pragma once

#include "Rendering/Uniforms/IShaderResource.hpp"
#include <vector>

namespace Beer::Rendering
{
    struct BufferBinding
    {
    public:
        size_t Size{};
        uint32_t Binding{};
        uint32_t DescriptorCount{};

        BufferBinding(size_t size, uint32_t binding, uint32_t descriptorCount)
            : Size(size), Binding(binding), DescriptorCount(descriptorCount)
        {
        }
    };

    class GlobalBuffer : public IShaderResource
    {
    private:
        std::vector<BufferBinding> bufferBindings;
        size_t minAlignment;

    public:
        GlobalBuffer(std::vector<BufferBinding> bufferBinding);
        void Update(uint32_t binding, const void* data);

    protected:
        std::vector<vk::DescriptorSetLayoutBinding> GetBindings() override;

    private:
        size_t GetTotalSize();
    };
} // namespace Beer::Rendering
