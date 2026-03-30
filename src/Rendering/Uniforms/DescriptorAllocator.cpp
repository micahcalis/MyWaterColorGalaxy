#include "Rendering/Uniforms/DescriptorAllocator.hpp"
#include "DescriptorAllocator.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    constexpr uint32_t DESCRIPTOR_COUNT = 1000;
    constexpr std::array<vk::DescriptorPoolSize, 4> POOL_SIZES = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, DESCRIPTOR_COUNT),
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, DESCRIPTOR_COUNT),

        vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, DESCRIPTOR_COUNT),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, DESCRIPTOR_COUNT)};

    DescriptorAllocator::DescriptorAllocator(const uint32_t framesInFlight, const Core::Device* device)
        : FramesInFlight(framesInFlight)
        , Device(device)
    {
        CreatePool();
    }

    void DescriptorAllocator::CreatePool()
    {
        vk::DescriptorPoolCreateInfo poolInfo(
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            DESCRIPTOR_COUNT,
            POOL_SIZES);

        currentPools.emplace_back(vk::raii::DescriptorPool(Device->GetLogicalDevice(),
            poolInfo));

        currentPoolIndex++;
    }

    vk::raii::DescriptorSet DescriptorAllocator::Allocate(vk::DescriptorSetLayout layout)
    {
        vk::DescriptorSetAllocateInfo allocInfo(GetCurrentPool(), 1, &layout);

        try
        {
            auto sets = Device->GetLogicalDevice().allocateDescriptorSets(allocInfo);
            return std::move(sets.front());
        } catch (const vk::OutOfPoolMemoryError&)
        {
        } catch (const vk::FragmentedPoolError&)
        {
        }

        CreatePool();
        allocInfo.descriptorPool = GetCurrentPool();

        auto sets = Device->GetLogicalDevice().allocateDescriptorSets(allocInfo);
        return std::move(sets.front());
    }
} // namespace Beer::Rendering
