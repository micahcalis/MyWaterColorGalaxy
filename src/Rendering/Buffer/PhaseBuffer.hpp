#pragma once

#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Pipeline/Frame/Resource/IRenderResource.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/BufferSyncState.hpp"

namespace Beer::Rendering
{
    class PhaseBuffer : public IRenderResource
    {
        friend class FrameBlackbox;
        friend class FrameGraph;

    private:
        std::shared_ptr<Buffer> bufferHandle;
        std::string name;
        void* mappedData;

    public:
        PhaseBuffer(const std::string& name,
            std::shared_ptr<Buffer> buffer);

        VkDeviceSize Size() const { return bufferHandle->GetData().Size; }
        std::string Name() const { return name; }
        [[nodiscard]] Buffer* GetHandle() const { return bufferHandle.get(); }
        void* GetMappedPointer() const { return bufferHandle->GetAllocInfo().pMappedData; }
        void UploadAsync(void* data, size_t size, size_t offset = 0);

    private:
        void SetBuffer(std::shared_ptr<Buffer> buffer);
        std::unique_ptr<ISyncBarrier> GetBarrier(const ResourceAction action) override;

        BufferSyncState* GetSyncState()
        {
            return static_cast<BufferSyncState*>(syncState.get());
        }

    private:
        inline static std::shared_ptr<PhaseBuffer> bufferFallbackStandard;
        inline static std::shared_ptr<PhaseBuffer> bufferFallbackDynamic;

    public:
        static void InitializeFallbackBuffer();
        static void DestroyFallbackBuffer();
        static PhaseBuffer* GetFallbackBuffer(bool isDynamic) { return isDynamic ? bufferFallbackDynamic.get() : bufferFallbackStandard.get(); }
        static VkDeviceSize CalculateSize(uint32_t count, size_t size);
    };
} // namespace Beer::Rendering
