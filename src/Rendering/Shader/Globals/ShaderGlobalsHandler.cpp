#include "Rendering/Shader/Globals/ShaderGlobalsHandler.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"

namespace Beer::Rendering
{
    ShaderGlobalsHandler::ShaderGlobalsHandler(const Core::Device* device, uint32_t framesInFlight)
    {
        Initialize(device->GetLogicalDevice(), framesInFlight);
        container = std::make_unique<ShaderGlobalsContainer>(framesInFlight);
    }

    void ShaderGlobalsHandler::Update()
    {
    }

    std::vector<vk::DescriptorSetLayoutBinding> ShaderGlobalsHandler::GetBindings() const
    {
        return {vk::DescriptorSetLayoutBinding(
            EngineGlobals::BINDING,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)};
    }

    std::vector<vk::DescriptorPoolSize> ShaderGlobalsHandler::GetPoolSizes(uint32_t framesInFlight) const
    {
        return {vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBuffer,
            framesInFlight)};
    }
} // namespace Beer::Rendering
