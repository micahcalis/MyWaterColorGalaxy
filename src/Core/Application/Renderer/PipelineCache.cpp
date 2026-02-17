#include "Core/Application/Renderer/PipelineCache.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Application/Renderer/PipelineKey.hpp"
#include "PipelineCache.hpp"
#include <cstdint>
#include <vulkan/vulkan.h>
#include "Rendering/Vertex.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    PipelineCache::PipelineCache(const vk::raii::Device& device,
        const Swapchain& swapchain,
        vk::DescriptorSetLayout globalSetLayout)
        : logicalDevice(device)
        , swapchain(swapchain)
        , globalSetLayout(globalSetLayout)
    {
    }

    const vk::raii::Pipeline& PipelineCache::GetPipeline(const PipelineKey& key, const PipelineData& data)
    {
        auto lookUp = cache.find(key);

        if (lookUp != cache.end())
        {
            return lookUp->second;
        }

        auto result = cache.emplace(key, CreatePipeline(data));
        return result.first->second;
    }

    vk::raii::Pipeline PipelineCache::CreatePipeline(const PipelineData& data)
    {
        auto shaderCode = AssetUtilities::ReadFile(data.ShaderPath);
        auto shaderModule = AssetUtilities::CreateShaderModule(shaderCode,
            logicalDevice);

        std::string vertexEntryName = data.GetVertexName();
        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertShaderStageInfo.module = shaderModule;
        vertShaderStageInfo.pName = vertexEntryName.c_str();

        std::string fragmentEntryName = data.GetFragmentName();
        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragShaderStageInfo.module = shaderModule;
        fragShaderStageInfo.pName = fragmentEntryName.c_str();

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        auto bindingDescription = Rendering::Vertex::GetBindingDescription();
        auto attributeDescriptions = Rendering::Vertex::GetAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicCreateInfo{};
        dynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicCreateInfo.pDynamicStates = dynamicStates.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
        inputAssemblyCreateInfo.topology = data.Topology;

        vk::PipelineViewportStateCreateInfo viewportCreateInfo{};
        viewportCreateInfo.viewportCount = 1;
        viewportCreateInfo.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
        rasterizationCreateInfo.depthClampEnable = vk::False;
        rasterizationCreateInfo.rasterizerDiscardEnable = vk::False;
        rasterizationCreateInfo.polygonMode = data.PolygonMode;
        rasterizationCreateInfo.cullMode = data.CullMode;
        rasterizationCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizationCreateInfo.depthBiasEnable = vk::False;
        rasterizationCreateInfo.depthBiasSlopeFactor = 1.0f;
        rasterizationCreateInfo.lineWidth = 1.0f;

        vk::PipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
        multisamplingCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisamplingCreateInfo.sampleShadingEnable = vk::False;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = data.UseTransparency ? vk::True : vk::False;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;

        vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
        colorBlendCreateInfo.logicOpEnable = vk::False;
        colorBlendCreateInfo.logicOp = vk::LogicOp::eCopy;
        colorBlendCreateInfo.attachmentCount = 1;
        colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

        std::vector<vk::DescriptorSetLayout> setLayouts;

        setLayouts.push_back(globalSetLayout);

        if (data.MaterialLayout != VK_NULL_HANDLE)
        {
            setLayouts.push_back(data.MaterialLayout);
        }

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        layoutCreateInfo.pSetLayouts = setLayouts.data();

        // NOTE: needs caching
        vk::raii::PipelineLayout finalLayout(logicalDevice, layoutCreateInfo);

        vk::PipelineRenderingCreateInfo renderingCreateInfo{};
        vk::Format colorFormat = swapchain.GetImageFormat();
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &colorFormat;

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
        graphicsPipelineCreateInfo.pDynamicState = &dynamicCreateInfo;
        graphicsPipelineCreateInfo.layout = *finalLayout;
        graphicsPipelineCreateInfo.renderPass = nullptr;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        vk::raii::Pipeline pipeline(logicalDevice, nullptr, graphicsPipelineCreateInfo);
        cachedLayouts.push_back(std::move(finalLayout));

        return pipeline;
    }
} // namespace Beer::Core
