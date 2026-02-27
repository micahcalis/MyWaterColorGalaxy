#include "Rendering/Shader/Shader.hpp"
#include <filesystem>
#include <stdexcept>
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Rendering/Shader/ShaderReflection.hpp"
#include "ShaderPass.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "vulkan/vulkan.hpp"
#include "Rendering/Vertex.hpp"

namespace Beer::Rendering
{
    Shader::Shader(const std::filesystem::path shaderPath,
        const std::filesystem::path jsonPath,
        const Core::Device& device,
        const Core::Swapchain& swapchain)
    {
        if (!GlobalInitialized())
            throw std::runtime_error("Shader Globals not initialized, can't create Shader");

        auto spvCode = Core::AssetUtilities::LoadSpvFile(shaderPath);
        materialProperties = Rendering::ShaderReflection::ReflectProperties(spvCode);
        CreateMaterialSetLayout(device);
        InitializeLayout(device);

        std::vector<PassSettings> passesSettings = Rendering::ShaderReflection::ReflectSettingsJson(jsonPath);
        auto shaderCode = Core::AssetUtilities::ReadFile(shaderPath);
        auto shaderModule = Core::AssetUtilities::CreateShaderModule(shaderCode,
            device.GetLogicalDevice());

        for (const auto settings : passesSettings)
        {
            // passes.try_emplace({settings.Type},
            //     ShaderPass(CreatePipeline(settings, shaderModule, device, swapchain),
            //         settings));
        }
    }

    void Shader::CreateMaterialSetLayout(const Core::Device& device)
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        for (const auto& [name, prop] : materialProperties)
        {
            vk::DescriptorSetLayoutBinding binding{};
            binding.binding = prop.Binding;
            binding.descriptorCount = 1;

            binding.stageFlags = vk::ShaderStageFlagBits::eAllGraphics;

            if (prop.Type == PropertyType::Texture2D)
            {
                binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            } else
            {
                binding.descriptorType = vk::DescriptorType::eUniformBuffer;
            }

            bindings.push_back(binding);
        }

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        materialSetLayout = vk::raii::DescriptorSetLayout(device.GetLogicalDevice(), layoutInfo);
    }

    void Shader::InitializeLayout(const Core::Device& device)
    {
        std::vector<vk::DescriptorSetLayout> setLayouts;

        setLayouts.push_back(globalSetLayout);
        setLayouts.push_back(materialSetLayout);

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        layoutCreateInfo.pSetLayouts = setLayouts.data();

        pipelineLayout = vk::raii::PipelineLayout(device.GetLogicalDevice(), layoutCreateInfo);
    }

    vk::Pipeline Shader::CreatePipeline(const PassSettings settings,
        const vk::ShaderModule shaderModule,
        const Core::Device& device,
        const Core::Swapchain& swapchain)
    {
        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertShaderStageInfo.module = shaderModule;
        vertShaderStageInfo.pName = settings.Vertex.c_str();

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragShaderStageInfo.module = shaderModule;
        fragShaderStageInfo.pName = settings.Fragment.c_str();

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        auto bindingDescriptions = Rendering::Vertex::GetBindingDescriptions();
        auto attributeDescriptions = Rendering::Vertex::GetAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicCreateInfo{};
        dynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicCreateInfo.pDynamicStates = dynamicStates.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
        inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

        vk::PipelineViewportStateCreateInfo viewportCreateInfo{};
        viewportCreateInfo.viewportCount = 1;
        viewportCreateInfo.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
        rasterizationCreateInfo.depthClampEnable = vk::False;
        rasterizationCreateInfo.rasterizerDiscardEnable = vk::False;
        rasterizationCreateInfo.polygonMode = settings.PolygonMode;
        rasterizationCreateInfo.cullMode = settings.CullMode;
        rasterizationCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizationCreateInfo.depthBiasEnable = vk::False;
        rasterizationCreateInfo.depthBiasSlopeFactor = 1.0f;
        rasterizationCreateInfo.lineWidth = 1.0f;

        vk::PipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
        multisamplingCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisamplingCreateInfo.sampleShadingEnable = vk::False;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = settings.Blend ? vk::True : vk::False;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.srcColorBlendFactor = settings.SrcBlend;
        colorBlendAttachment.dstColorBlendFactor = settings.DstBlend;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;

        vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
        colorBlendCreateInfo.logicOpEnable = vk::False;
        colorBlendCreateInfo.logicOp = vk::LogicOp::eCopy;
        colorBlendCreateInfo.attachmentCount = 1;
        colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

        vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
        depthStencilCreateInfo.depthTestEnable = settings.DepthTest ? vk::True : vk::False;
        depthStencilCreateInfo.depthWriteEnable = settings.DepthWrite ? vk::True : vk::False;
        depthStencilCreateInfo.depthCompareOp = settings.CompareOp;
        depthStencilCreateInfo.depthBoundsTestEnable = vk::False;
        depthStencilCreateInfo.stencilTestEnable = vk ::False;

        vk::PipelineRenderingCreateInfo renderingCreateInfo{};
        vk::Format colorFormat = swapchain.GetImageFormat();
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &colorFormat;
        renderingCreateInfo.depthAttachmentFormat = depthFormat;

        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.pNext = &renderingCreateInfo;
        graphicsPipelineCreateInfo.stageCount = 2;
        graphicsPipelineCreateInfo.pStages = &shaderStages[0];
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
        graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicCreateInfo;
        graphicsPipelineCreateInfo.layout = *pipelineLayout;
        graphicsPipelineCreateInfo.renderPass = nullptr;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        vk::raii::Pipeline pipeline(device.GetLogicalDevice(), nullptr, graphicsPipelineCreateInfo);
        return pipeline;
    }
} // namespace Beer::Rendering
