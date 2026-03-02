#pragma once

#include "ShaderGlobalsContainer.hpp"
#include <memory>
#include "Rendering/Uniforms/IDescriptorHandler.hpp"
#include "Core/Application/Renderer/Device.hpp"

namespace Beer::Rendering
{
    class ShaderGlobalsHandler : public IDescriptorHandler
    {
    private:
        std::unique_ptr<ShaderGlobalsContainer> container = nullptr;

    public:
        ShaderGlobalsHandler(const Core::Device* device, uint32_t framesInFlight);
        ShaderGlobalsContainer* GetContainer() { return container.get(); }
        void Update();

    protected:
        std::vector<vk::DescriptorSetLayoutBinding> GetBindings() const override;
        std::vector<vk::DescriptorPoolSize> GetPoolSizes(uint32_t framesInFlight) const override;
    };
} // namespace Beer::Rendering
