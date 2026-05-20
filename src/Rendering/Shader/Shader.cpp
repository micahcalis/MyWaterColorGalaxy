#include "Rendering/Shader/Shader.hpp"
#include <filesystem>
#include <print>
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "FragmentOutput.hpp"
#include "ModelPush.hpp"
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
        const Core::Device& device)
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
        shaderModule = Core::AssetUtilities::CreateShaderModule(shaderCode,
            device.GetLogicalDevice());

        for (const auto settings : passesSettings)
        {
            VertexInput passInput = ShaderReflection::ReflectVertexInput(spvCode, settings.Vertex);
            FragmentTemplate passOutput = ShaderReflection::ReflectFragment(spvCode, settings.Fragment);

            passes.try_emplace(settings.Type,
                settings,
                passInput,
                passOutput);
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

    const vk::Pipeline Shader::GetPipeline(const ShaderPass* pass, const FragmentOutput output) const
    {
        auto it = pass->PipelineMap.find(output);
        if (it == pass->PipelineMap.end())
        {
            if (pass->FragTemplate.OutputCompatible(output))
            {
                vk::raii::Pipeline newPipeline = CreatePipeline(pass->Settings,
                    pass->Input,
                    pass->FragTemplate,
                    output,
                    *shaderModule,
                    shaderManager->GetDevice());

                auto insertResult = pass->PipelineMap.emplace(output, std::move(newPipeline));
                it = insertResult.first;
            } else
            {
                PrintConfig();
                throw std::runtime_error("FragmentOutput formats do not match Shader's SPIR-V Template!");
            }
        }

        return *it->second;
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

        setLayouts.append_range(Shader::Globals()->GetGlobalsLayout());
        setLayouts.push_back(Shader::Globals()->GetTransformLayout());
        setLayouts.push_back(materialSetLayout);

        vk::PushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(Rendering::ModelPush);

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        layoutCreateInfo.pSetLayouts = setLayouts.data();
        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &pushConstantRange;

        pipelineLayout = vk::raii::PipelineLayout(device.GetLogicalDevice(), layoutCreateInfo);
    }

    vk::raii::Pipeline Shader::CreatePipeline(const PassSettings& settings,
        const VertexInput& input,
        const FragmentTemplate& fragTemplate,
        const FragmentOutput& output,
        const vk::ShaderModule shaderModule,
        const Core::Device* device) const
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
        inputAssemblyCreateInfo.topology = settings.TopologyMode;

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

        std::vector<vk::PipelineColorBlendAttachmentState> colorAttachments;
        for (const auto& fragPair : fragTemplate.TemplateList)
        {
            if (fragPair.Type == FragOutputType::Depth)
            {
                continue;
            }

            vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.blendEnable = settings.Blend ? vk::True : vk::False;
            colorBlendAttachment.colorWriteMask = fragPair.ColorComponents;
            colorBlendAttachment.srcColorBlendFactor = settings.SrcBlend;
            colorBlendAttachment.dstColorBlendFactor = settings.DstBlend;
            colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;

            colorAttachments.push_back(colorBlendAttachment);
        }

        vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
        colorBlendCreateInfo.logicOpEnable = vk::False;
        colorBlendCreateInfo.logicOp = vk::LogicOp::eCopy;

        colorBlendCreateInfo.attachmentCount = static_cast<uint32_t>(colorAttachments.size());
        colorBlendCreateInfo.pAttachments = colorAttachments.data();

        vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
        depthStencilCreateInfo.depthTestEnable = settings.DepthTest ? vk::True : vk::False;
        depthStencilCreateInfo.depthWriteEnable = settings.DepthWrite ? vk::True : vk::False;
        depthStencilCreateInfo.depthCompareOp = settings.CompareOp;
        depthStencilCreateInfo.depthBoundsTestEnable = vk::False;
        depthStencilCreateInfo.stencilTestEnable = vk::False;

        vk::PipelineRenderingCreateInfo renderingCreateInfo{};
        renderingCreateInfo.colorAttachmentCount = static_cast<uint32_t>(output.ColorFormats.size());
        renderingCreateInfo.pColorAttachmentFormats = output.ColorFormats.data();
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

        vk::raii::Pipeline pipeline(device->GetLogicalDevice(), nullptr, graphicsPipelineCreateInfo);
        return pipeline;
    }

    void Shader::PrintConfig() const
    {
        for (auto& pass : passes)
        {
            std::println("Pass Type: {}", magic_enum::enum_name(pass.second.Settings.Type));
            pass.second.Input.BufferOrder.Print();
            pass.second.FragTemplate.Print();
        }

        materialProperties->Print();
    }
} // namespace Beer::Rendering
