#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/SyncTargetState.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    void BufferSyncBarrier::GenerateBarrier(const ResourceAction action)
    {
        SyncTargetState targetState = SyncTargetState::GetState(action);

        srcStage = state->CurrentStage;
        dstStage = targetState.Stage;

        barrier = vk::BufferMemoryBarrier{};
        barrier.srcAccessMask = state->CurrentAccess;
        barrier.dstAccessMask = targetState.Access;
        barrier.buffer = state->BufferHandle->GetHandle();

        BufferData data = state->BufferHandle->GetData();
        barrier.offset = data.Offset;
        barrier.size = data.Size;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        state->CurrentAccess = targetState.Access;
        state->CurrentStage = targetState.Stage;
        state->CurrentAction = action;
    }

    void BufferSyncBarrier::RecordBarrier(CommandBuffer* commandBuffer)
    {
        commandBuffer->RecordBufferBarrier(srcStage,
            dstStage,
            barrier);
    }
} // namespace Beer::Rendering
