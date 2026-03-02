#include "Rendering/Material/IDescriptorHandler.hpp"

namespace Beer::Rendering
{
    void IDescriptorHandler::Initialize(const vk::raii::Device& logicalDevice, uint32_t framesInFlight)
    {
        auto bindings = GetBindings();
        vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
        layout = vk::raii::DescriptorSetLayout(logicalDevice, layoutInfo);

        auto poolSizes = GetPoolSizes(framesInFlight);
        vk::DescriptorPoolCreateInfo poolInfo(
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            framesInFlight,
            poolSizes);
        pool = vk::raii::DescriptorPool(logicalDevice, poolInfo);

        std::vector<vk::DescriptorSetLayout> layouts(framesInFlight, *layout);
        vk::DescriptorSetAllocateInfo allocInfo(*pool, layouts);
        descriptorSets = logicalDevice.allocateDescriptorSets(allocInfo);
    }
} // namespace Beer::Rendering
