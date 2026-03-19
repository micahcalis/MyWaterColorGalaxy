#pragma once

#include "Rendering/Pipeline/Frame/Synchronization/ISyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncState.hpp"

namespace Beer::Rendering
{
    class ImageSyncBarrier : public ISyncBarrier
    {
    private:
        ImageSyncState* state;
        vk::ImageMemoryBarrier barrier;

    public:
        ImageSyncBarrier(ImageSyncState* state)
            : state(state)
        {
        }

        void GenerateBarrier(const ResourceAction action) override;
        void RecordBarrier(CommandBuffer* commandBuffer) override;
    };
} // namespace Beer::Rendering
