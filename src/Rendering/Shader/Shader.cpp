#include "Rendering/Shader/Shader.hpp"
#include <filesystem>
#include <print>
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Rendering/Shader/ShaderReflection.hpp"
#include "ShaderPass.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "VertexInput.hpp"
#include "vulkan/vulkan.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include "Core/Application/Managers/ShaderManager.hpp"

namespace Beer::Rendering
{
    Shader::Shader(const std::filesystem::path& shaderPath,
        const std::filesystem::path& jsonPath,
        const Core::Device& device,
        const Core::Swapchain& swapchain)
    {
        auto spvCode = Core::AssetUtilities::LoadSpvFile(shaderPath);

        uint32_t propertyBufferSize;
        auto propertyMap = Rendering::ShaderReflection::ReflectProperties(spvCode, propertyBufferSize);
        auto bindings = ShaderReflection::ReflectMaterialBindings(spvCode);
        materialProperties = std::make_unique<MaterialProperties>(
            std::move(propertyMap),
            propertyBufferSize,
            std::move(bindings));

        CreateMaterialSetLayout(device);
        InitializeLayout(device);

        std::vector<PassSettings> passesSettings = Rendering::ShaderReflection::ReflectSettingsJson(jsonPath);
        auto shaderCode = Core::AssetUtilities::ReadFile(shaderPath);
        auto shaderModule = Core::AssetUtilities::CreateShaderModule(shaderCode,
            device.GetLogicalDevice());

        for (const auto settings : passesSettings)
        {
            VertexInput passInput = ShaderReflection::ReflectVertexInput(spvCode, settings.Vertex);
            passes.try_emplace(
                settings.Type,
                CreatePipeline(settings, passInput, shaderModule, device, swapchain),
                settings,
                passInput.BufferOrder);
        }
    }

    std::shared_ptr<Shader> Shader::Get(const std::string& name)
    {
        return shaderManager->Get(name);
    }

    ShaderGlobalsHandler* Shader::Globals()
    {
        return shaderManager->GetGlobalsHandler();
    }

    void Shader::CreateMaterialSetLayout(const Core::Device& device)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = static_cast<uint32_t>(materialProperties->GetBindings().size());
        layoutInfo.pBindings = materialProperties->GetBindings().data();

        materialSetLayout = vk::raii::DescriptorSetLayout(device.GetLogicalDevice(), layoutInfo);
    }

    void Shader::InitializeLayout(const Core::Device& device)
    {
        std::vector<vk::DescriptorSetLayout> setLayouts;

        setLayouts.append_range(Shader::Globals()->GetLayouts());
        setLayouts.push_back(materialSetLayout);

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        layoutCreateInfo.pSetLayouts = setLayouts.data();

        pipelineLayout = vk::raii::PipelineLayout(device.GetLogicalDevice(), layoutCreateInfo);
    }

    vk::raii::Pipeline Shader::CreatePipeline(const PassSettings& settings,
        const VertexInput& input,
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

        auto bindingDescriptions = input.BindingDescs;
        auto attributeDescriptions = input.AttributeDescs;
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
        renderingCreateInfo.depthAttachmentFormat = Shader::shaderManager->GetDepthFormat();

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

    void Shader::PrintConfig()
    {
        for (auto& pass : passes)
        {
            std::println("Pass Type: {}", magic_enum::enum_name(pass.second.Settings.Type));
            pass.second.BufferOrder.Print();
        }

        materialProperties->Print();
    }
} // namespace Beer::Rendering
