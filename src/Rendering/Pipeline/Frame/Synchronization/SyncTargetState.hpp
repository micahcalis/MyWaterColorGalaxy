#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct SyncTargetState
    {
    public:
        vk::ImageLayout Layout;
        vk::AccessFlags Access;
        vk::PipelineStageFlags Stage;

    public:
        static SyncTargetState GetState(const ResourceAction action)
        {
            switch (action)
            {
            case ResourceAction::ColorRead:
                return {vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader};

            case ResourceAction::ColorWrite:
                return {vk::ImageLayout::eColorAttachmentOptimal,
                    vk::AccessFlagBits::eColorAttachmentWrite,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput};

            case ResourceAction::DepthWrite:
                return {vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead,
                    vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests};

            case ResourceAction::ComputeRead:
                return {vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eComputeShader};

            case ResourceAction::ComputeWrite:
                return {vk::ImageLayout::eGeneral,
                    vk::AccessFlagBits::eShaderWrite,
                    vk::PipelineStageFlagBits::eComputeShader};

            case ResourceAction::ComputeReadWrite:
                return {vk::ImageLayout::eGeneral,
                    vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite,
                    vk::PipelineStageFlagBits::eComputeShader};

            default:
                return {
                    vk::ImageLayout::eUndefined,
                    vk::AccessFlags(0),
                    vk::PipelineStageFlagBits::eTopOfPipe};
            }
        }
    };
} // namespace Beer::Rendering
