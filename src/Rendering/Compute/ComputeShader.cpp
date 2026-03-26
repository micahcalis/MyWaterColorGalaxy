#include "Rendering/Compute/ComputeShader.hpp"
#include "ComputeKernel.hpp"
#include "Core/Application/Managers/ComputeManager.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include <stdexcept>
#include "Rendering/Shader/ShaderReflection.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    std::shared_ptr<ComputeShader> ComputeShader::Get(const std::string& name)
    {
        return computeManager->Get(name);
    }

    ComputeShader::ComputeShader(const std::filesystem::path& shaderPath,
        const std::filesystem::path& jsonPath,
        const Core::Device& device,
        const Core::Swapchain& swapchain)
    {
        auto spvCode = Core::AssetUtilities::LoadSpvFile(shaderPath);

        uint32_t propertyBufferSize;
        auto propertyMap = Rendering::ShaderReflection::ReflectProperties(spvCode, propertyBufferSize);
        auto bindings = ShaderReflection::ReflectMaterialBindings(spvCode, true);
        materialProperties = std::make_unique<MaterialProperties>(
            std::move(propertyMap),
            propertyBufferSize,
            std::move(bindings));

        CreateMaterialSetLayout(device);
        InitializeLayout(device);

        std::vector<KernelSettings> kernelsSettings = Rendering::ShaderReflection::ReflectKernelsJson(jsonPath);
        auto shaderCode = Core::AssetUtilities::ReadFile(shaderPath);
        auto shaderModule = Core::AssetUtilities::CreateShaderModule(shaderCode,
            device.GetLogicalDevice());

        for (const auto settings : kernelsSettings)
        {
            kernelIndexMap[settings.Name] = settings.Index;
            kernels.emplace_back(CreatePipeline(settings.Name, shaderModule, device),
                settings);
        }
    }

    const ComputeKernel* ComputeShader::GetKernel(const uint32_t kernelIndex) const
    {
        if (kernelIndex >= kernels.size())
        {
            throw std::runtime_error(
                std::format("Compute Kernel Out of Range: {}", kernelIndex));
        }

        return &kernels[kernelIndex];
    }

    uint32_t ComputeShader::GetKernelIndex(const std::string& kernelName) const
    {
        auto it = kernelIndexMap.find(kernelName);

        if (it == kernelIndexMap.end())
        {
            return -1;
        }

        return it->second;
    }

    void ComputeShader::CreateMaterialSetLayout(const Core::Device& device)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = static_cast<uint32_t>(materialProperties->GetBindings().size());
        layoutInfo.pBindings = materialProperties->GetBindings().data();

        materialSetLayout = vk::raii::DescriptorSetLayout(device.GetLogicalDevice(), layoutInfo);
    }

    void ComputeShader::InitializeLayout(const Core::Device& device)
    {
        std::vector<vk::DescriptorSetLayout> setLayouts;

        setLayouts.append_range(Shader::Globals()->GetLayouts());
        setLayouts.push_back(materialSetLayout);

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        layoutCreateInfo.pSetLayouts = setLayouts.data();

        pipelineLayout = vk::raii::PipelineLayout(device.GetLogicalDevice(), layoutCreateInfo);
    }

    vk::raii::Pipeline ComputeShader::CreatePipeline(const std::string& kernelName,
        const vk::ShaderModule shaderModule,
        const Core::Device& device)
    {
        vk::PipelineShaderStageCreateInfo computeShaderStageCreateInfo{};
        computeShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eCompute;
        computeShaderStageCreateInfo.module = shaderModule;
        computeShaderStageCreateInfo.pName = kernelName.c_str();

        vk::ComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.stage = computeShaderStageCreateInfo;
        computePipelineCreateInfo.layout = *pipelineLayout;

        vk::raii::Pipeline pipeline(device.GetLogicalDevice(), nullptr, computePipelineCreateInfo);
        return pipeline;
    }

    void ComputeShader::PrintConfig()
    {
        for (auto& kernel : kernels)
        {
            std::println("Kernel Name: {}", kernel.Settings.Name);
        }

        materialProperties->Print();
    }
} // namespace Beer::Rendering
