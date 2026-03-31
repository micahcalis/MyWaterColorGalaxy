#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/Frame/Resource/RenderResourceType.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncState.hpp"
#include <memory>
#include <utility>

namespace Beer::Rendering
{
    constexpr VkDeviceSize FALLBACK_SIZE = 1000000;

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

    void PhaseBuffer::InitializeFallbackBuffer()
    {
        std::shared_ptr<Buffer> bufferHandle = std::make_shared<Buffer>(Buffer::CreateSSBO(FALLBACK_SIZE));
        bufferFallback = std::make_shared<PhaseBuffer>("FallbackBuffer", std::move(bufferHandle));
    }

    void PhaseBuffer::DestroyFallbackBuffer()
    {
        bufferFallback.reset();
    }
} // namespace Beer::Rendering
