#pragma once

#include "ShaderPass.hpp"
#include <filesystem>
#include <unordered_map>
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::Rendering
{
    class Shader
    {
    private:
        std::unordered_map<ShaderPassType, ShaderPass> passes;
        std::unordered_map<std::string, ShaderProperty> materialProperties;

        vk::PipelineLayout pipelineLayout;
        vk::DescriptorSetLayout materialSetLayout;

    public:
        Shader(const std::filesystem::path path, const Core::Device& device);
        ~Shader();

        const ShaderPass GetPass(ShaderPassType passType) const;
        const ShaderProperty* GetShaderProperty(std::string& propertyName) const;

        vk::DescriptorSetLayout GetMaterialSetLayout() const { return materialSetLayout; }
        vk::PipelineLayout GetPipelineLayout() const { return pipelineLayout; }
    };
} // namespace Beer::Rendering
