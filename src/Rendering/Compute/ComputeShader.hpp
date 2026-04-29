#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>
#include "ComputeKernel.hpp"
#include "Rendering/Compute/ComputeKernel.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Rendering/Material/MaterialProperties.hpp"

namespace Beer::Core
{
    class ComputeManager;
}

namespace Beer::Rendering
{
    class ComputeShader
    {
    private:
        inline static Core::ComputeManager* computeManager = nullptr;

        std::unordered_map<std::string, uint32_t> kernelIndexMap;
        std::vector<ComputeKernel> kernels;
        std::unique_ptr<MaterialProperties> materialProperties;
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::DescriptorSetLayout materialSetLayout = nullptr;

    public:
        static void SetComputeManager(Core::ComputeManager* computeManager)
        {
            ComputeShader::computeManager = computeManager;
        }

        static std::shared_ptr<ComputeShader> Get(const std::string& name);

        ComputeShader(const std::filesystem::path& shaderPath,
            const std::filesystem::path& jsonPath,
            const Core::Device& device,
            const Core::Swapchain& swapchain);

        const ComputeKernel* GetKernel(const uint32_t kernelIndex) const;
        uint32_t GetKernelIndex(const std::string& kernelName) const;

        vk::DescriptorSetLayout GetMaterialSetLayout() const { return materialSetLayout; }
        vk::PipelineLayout GetPipelineLayout() const { return *pipelineLayout; }
        MaterialProperties* GetProperties() const { return materialProperties.get(); }

        void PrintConfig() const;

    private:
        void CreateMaterialSetLayout(const Core::Device& device);
        void InitializeLayout(const Core::Device& device);
        vk::raii::Pipeline CreatePipeline(const std::string& kernelName,
            const vk::ShaderModule shaderModule,
            const Core::Device& device);
    };
} // namespace Beer::Rendering
