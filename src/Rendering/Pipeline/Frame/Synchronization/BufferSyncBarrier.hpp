#pragma once

#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncState.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ISyncBarrier.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class BufferSyncBarrier : public ISyncBarrier
    {
    private:
        BufferSyncState* state;
        vk::BufferMemoryBarrier barrier;

    public:
        BufferSyncBarrier(BufferSyncState* state)
            : state(state)
        {
        }

        void GenerateBarrier(const ResourceAction action) override;
        void RecordBarrier(CommandBuffer* commandBuffer) override;
    };
} // namespace Beer::Rendering
