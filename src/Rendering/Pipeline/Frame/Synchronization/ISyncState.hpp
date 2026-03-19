#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/Frame/IRenderResource.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct ISyncState
    {
    public:
        vk::PipelineStageFlags CurrentStage = vk::PipelineStageFlagBits::eTopOfPipe;
        vk::AccessFlags CurrentAccess = vk::AccessFlags(0);

    public:
        virtual ~ISyncState() = default;
    };
} // namespace Beer::Rendering
