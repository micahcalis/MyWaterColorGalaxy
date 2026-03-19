#pragma once

#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class ISyncBarrier
    {
    protected:
        vk::PipelineStageFlags srcStage;
        vk::PipelineStageFlags dstStage;

    public:
        virtual ~ISyncBarrier() = default;

        virtual void GenerateBarrier(const ResourceAction action) = 0;
        virtual void RecordBarrier(CommandBuffer* commandBuffer) = 0;
    };
} // namespace Beer::Rendering
