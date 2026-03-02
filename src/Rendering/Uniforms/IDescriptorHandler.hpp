#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vector>

namespace Beer::Rendering
{
    class IDescriptorHandler
    {
    protected:
        vk::raii::DescriptorSetLayout layout = nullptr;
        vk::raii::DescriptorPool pool = nullptr;
        std::vector<vk::raii::DescriptorSet> descriptorSets;

    public:
        virtual ~IDescriptorHandler() = default;
        void Initialize(const vk::raii::Device& logicalDevice, uint32_t framesInFlight);
        vk::DescriptorSetLayout GetLayout() const { return *layout; }
        vk::DescriptorSet GetSet(uint32_t frameIndex) const { return *descriptorSets[frameIndex]; }

    protected:
        virtual std::vector<vk::DescriptorSetLayoutBinding> GetBindings() const = 0;
        virtual std::vector<vk::DescriptorPoolSize> GetPoolSizes(uint32_t framesInFlight) const = 0;
    };
} // namespace Beer::Rendering
