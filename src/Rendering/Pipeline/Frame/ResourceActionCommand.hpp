#pragma once

#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ISyncBarrier.hpp"

namespace Beer::Rendering
{
    class ResourceActionCommand
    {
    private:
        std::unique_ptr<ISyncBarrier> syncBarrier;

    public:
        ResourceActionCommand(std::unique_ptr<ISyncBarrier> barrier)
            : syncBarrier(std::move(barrier))
        {
        }

        void Execute(CommandBuffer* commandBuffer)
        {
            syncBarrier->RecordBarrier(commandBuffer);
        }
    };
} // namespace Beer::Rendering
