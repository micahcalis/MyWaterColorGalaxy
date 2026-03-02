#pragma once

#include "Rendering/Shader/Globals/ShaderGlobalsHandler.hpp"
#include "ShaderPass.hpp"
#include <filesystem>
#include <unordered_map>
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "VertexInput.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    class ShaderManager;
}

namespace Beer::Rendering
{
    class Shader
    {
    private:
        inline static Core::ShaderManager* shaderManager = nullptr;

        std::unordered_map<ShaderPassType, ShaderPass> passes;
        std::unordered_map<std::string, ShaderProperty> materialProperties;

        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::DescriptorSetLayout materialSetLayout = nullptr;

    public:
        static void SetShaderManager(Core::ShaderManager* shaderManager)
        {
            Shader::shaderManager = shaderManager;
        }

        static std::shared_ptr<Shader> Get(const std::string& name);
        static ShaderGlobalsHandler* Globals();

        Shader(const std::filesystem::path& shaderPath,
            const std::filesystem::path& jsonPath,
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
        void BindShader(vk::CommandBuffer commandBuffer);
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
