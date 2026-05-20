#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/Frame/Resource/RenderResourceType.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncState.hpp"
#include "SSBOType.hpp"
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

    void PhaseBuffer::UploadAsync(void* data, size_t size, size_t offset)
    {
        Buffer::UploadAsync(bufferHandle, data, size, offset);
    }

    void PhaseBuffer::InitializeFallbackBuffer()
    {
        std::shared_ptr<Buffer> bufferHandleStandard = std::make_shared<Buffer>(Buffer::CreateSSBO(FALLBACK_SIZE, SSBOType::Hybrid));
        bufferFallbackStandard = std::make_shared<PhaseBuffer>("FallbackBufferStandard", std::move(bufferHandleStandard));

        std::shared_ptr<Buffer> bufferHandleDynamic = std::make_shared<Buffer>(Buffer::CreateDynamic(FALLBACK_SIZE, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));
        bufferFallbackDynamic = std::make_shared<PhaseBuffer>("FallbackBufferDynamic", std::move(bufferHandleDynamic));
    }

    void PhaseBuffer::DestroyFallbackBuffer()
    {
        bufferFallbackStandard.reset();
        bufferFallbackDynamic.reset();
    }

    VkDeviceSize PhaseBuffer::CalculateSize(uint32_t count, size_t size)
    {
        return static_cast<VkDeviceSize>(count * size);
    }
} // namespace Beer::Rendering
