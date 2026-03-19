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
            case ResourceAction::Read:
                return {
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader};

            case ResourceAction::Write: {
                return {vk::ImageLayout::eColorAttachmentOptimal,
                    vk::AccessFlagBits::eColorAttachmentWrite,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput};
            }

            default:
                return {
                    vk::ImageLayout::eUndefined,
                    vk::AccessFlags(0),
                    vk::PipelineStageFlagBits::eTopOfPipe};
            }
        }
    };
} // namespace Beer::Rendering
