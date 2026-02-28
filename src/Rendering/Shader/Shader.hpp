#pragma once

#include "ShaderPass.hpp"
#include <filesystem>
#include <unordered_map>
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "VertexInput.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class Shader
    {
    private:
        inline static vk::DescriptorSetLayout globalSetLayout = nullptr;
        inline static vk::Format depthFormat = vk::Format::eUndefined;

        std::unordered_map<ShaderPassType, ShaderPass> passes;
        std::unordered_map<std::string, ShaderProperty> materialProperties;

        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::DescriptorSetLayout materialSetLayout = nullptr;

    public:
        static void SetGlobalsLayout(vk::DescriptorSetLayout globalsLayout)
        {
            Shader::globalSetLayout = globalsLayout;
        }

        static void SetDepthFormat(vk::Format format)
        {
            Shader::depthFormat = format;
        }

        static bool GlobalInitialized()
        {
            return globalSetLayout != VK_NULL_HANDLE && depthFormat != vk::Format::eUndefined;
        }

        Shader(const std::filesystem::path shaderPath,
            const std::filesystem::path jsonPath,
            const Core::Device& device,
            const Core::Swapchain& swapchain);

        const ShaderPass* GetPass(ShaderPassType passType) const
        {
            auto it = passes.find(passType);
            if (it == passes.end())
            {
                return nullptr;
            }

            return &it->second;
        }

        const ShaderProperty* GetShaderProperty(const std::string& propertyName) const
        {
            auto it = materialProperties.find(propertyName);
            if (it == materialProperties.end())
            {
                return nullptr;
            }

            return &it->second;
        }

        vk::DescriptorSetLayout GetMaterialSetLayout() const
        {
            return materialSetLayout;
        }
        vk::PipelineLayout GetPipelineLayout() const { return *pipelineLayout; }

        void PrintConfig();

    private:
        void CreateMaterialSetLayout(const Core::Device& device);
        void InitializeLayout(const Core::Device& device);

        vk::raii::Pipeline CreatePipeline(const PassSettings& settings,
            const VertexInput& input,
            const vk::ShaderModule shaderModule,
            const Core::Device& device,
            const Core::Swapchain& swapchain);
    };
} // namespace Beer::Rendering
