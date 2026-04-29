#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/SyncTargetState.hpp"

namespace Beer::Rendering
{
    void ImageSyncBarrier::GenerateBarrier(const ResourceAction action)
    {
        SyncTargetState targetState = SyncTargetState::GetState(action);

        srcStage = state->CurrentStage;
        dstStage = targetState.Stage;

        barrier = vk::ImageMemoryBarrier{};
        barrier.oldLayout = state->CurrentLayout;
        barrier.newLayout = targetState.Layout;
        barrier.srcAccessMask = state->CurrentAccess;
        barrier.dstAccessMask = targetState.Access;
        barrier.image = state->Image->GetHandle();

        ImageData data = state->Image->GetData();

        barrier.subresourceRange.aspectMask = data.AspectMask;
        barrier.subresourceRange.baseMipLevel = data.BaseMipLevel;
        barrier.subresourceRange.levelCount = data.MipLevels;
        barrier.subresourceRange.baseArrayLayer = data.BaseArrayLayer;
        barrier.subresourceRange.layerCount = data.ArrayLayers;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        state->CurrentLayout = targetState.Layout;
        state->CurrentAccess = targetState.Access;
        state->CurrentStage = targetState.Stage;
        state->CurrentAction = action;
    }

    void ImageSyncBarrier::RecordBarrier(CommandBuffer* commandBuffer)
    {
        commandBuffer->RecordImageBarrier(srcStage,
            dstStage,
            barrier);
    }
} // namespace Beer::Rendering
