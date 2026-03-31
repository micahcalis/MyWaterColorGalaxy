#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/Frame/Resource/RenderResourceType.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncState.hpp"
#include <memory>

namespace Beer::Rendering
{
    PhaseBuffer::PhaseBuffer(const std::string& name,
        std::shared_ptr<Buffer> buffer)
        : name(name), bufferHandle(buffer)
    {
        type = RenderResourceType::Buffer;
        syncState = std::make_unique<BufferSyncState>(buffer.get());
    }

    std::unique_ptr<ISyncBarrier> PhaseBuffer::GetBarrier(const ResourceAction action)
    {
        std::unique_ptr<BufferSyncBarrier> barrier = std::make_unique<BufferSyncBarrier>(
            GetSyncState());

        barrier->GenerateBarrier(action);
        return barrier;
    }

    void PhaseBuffer::SetBuffer(std::shared_ptr<Buffer> buffer)
    {
        bufferHandle = std::move(buffer);
        syncState = std::make_unique<BufferSyncState>(bufferHandle.get());
    }

    static void InitializeFallbackBuffer();
} // namespace Beer::Rendering
