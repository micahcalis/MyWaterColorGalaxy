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

            case ResourceAction::DepthRead:
                return {vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eFragmentShader};

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

            case ResourceAction::BufferRead:
                return {vk::ImageLayout::eUndefined,
                    vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eUniformRead | vk::AccessFlagBits::eVertexAttributeRead | vk::AccessFlagBits::eIndexRead,
                    vk::PipelineStageFlagBits::eVertexInput | vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader};

            case ResourceAction::BufferWrite:
                return {vk::ImageLayout::eUndefined,
                    vk::AccessFlagBits::eShaderWrite,
                    vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader};

            case ResourceAction::BufferReadWrite:
                return {vk::ImageLayout::eUndefined,
                    vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite,
                    vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader};

            case ResourceAction::ComputeBufferRead:
                return {vk::ImageLayout::eUndefined,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eComputeShader};

            case ResourceAction::ComputeBufferWrite:
                return {vk::ImageLayout::eUndefined,
                    vk::AccessFlagBits::eShaderWrite,
                    vk::PipelineStageFlagBits::eComputeShader};

            case ResourceAction::ComputeBufferReadWrite:
                return {vk::ImageLayout::eUndefined,
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
