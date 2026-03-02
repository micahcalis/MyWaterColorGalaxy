#pragma once

#include "Rendering/Uniforms/IShaderResource.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"

namespace Beer::Rendering
{
    class EngineGlobalBuffer : public IShaderResource
    {
    public:
        EngineGlobalBuffer();
        void Update(const EngineGlobals& data);

    protected:
        std::vector<vk::DescriptorSetLayoutBinding> GetBindings() override;
    };
} // namespace Beer::Rendering
