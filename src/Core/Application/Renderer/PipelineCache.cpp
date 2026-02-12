#include "Core/Application/Renderer/PipelineCache.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Application/Renderer/PipelineKey.hpp"
#include "PipelineCache.hpp"
#include <vulkan/vulkan.h>

namespace Beer::Core
{
    PipelineCache::PipelineCache(const vk::raii::Device& device, const Swapchain& swapchain)
        : logicalDevice(device)
        , swapchain(swapchain)
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

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

        std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicCreateInfo{};
        dynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicCreateInfo.pDynamicStates = dynamicStates.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
        inputAssemblyCreateInfo.topology = data.Topology;

        const vk::Extent2D swapchainExtent = swapchain.GetExtent();
        vk::Viewport viewPort = vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f);
        vk::Rect2D region = vk::Rect2D(vk::Offset2D{0, 0}, swapchainExtent);

        vk::PipelineViewportStateCreateInfo viewportCreateInfo{};
        viewportCreateInfo.viewportCount = 1;
        viewportCreateInfo.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
        rasterizationCreateInfo.depthClampEnable = vk::False;
        rasterizationCreateInfo.rasterizerDiscardEnable = vk::False;
        rasterizationCreateInfo.polygonMode = data.PolygonMode;
        rasterizationCreateInfo.cullMode = data.CullMode;
        rasterizationCreateInfo.frontFace = vk::FrontFace::eClockwise;
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

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pushConstantRangeCount = 0;

        emptyLayout = vk::raii::PipelineLayout(logicalDevice, layoutCreateInfo);

        vk::PipelineRenderingCreateInfo renderingCreateInfo{};
        vk::Format colorFormat = swapchain.GetImageFormat();
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &colorFormat;

        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.pNext = &renderingCreateInfo;
        graphicsPipelineCreateInfo.stageCount = 2;
        graphicsPipelineCreateInfo.pStages = shaderStages;
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicCreateInfo;
        graphicsPipelineCreateInfo.layout = *emptyLayout;
        graphicsPipelineCreateInfo.renderPass = nullptr;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        return vk::raii::Pipeline(logicalDevice, nullptr, graphicsPipelineCreateInfo);
    }
} // namespace Beer::Core
