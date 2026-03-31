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

    public:
        PhaseBuffer(const std::string& name,
            std::shared_ptr<Buffer> buffer);

        VkDeviceSize Size() const { return bufferHandle->GetData().Size; }
        std::string Name() const { return name; }
        [[nodiscard]] Buffer* GetHandle() const { return bufferHandle.get(); }

    private:
        void SetBuffer(std::shared_ptr<Buffer> buffer);
        std::unique_ptr<ISyncBarrier> GetBarrier(const ResourceAction action) override;

        BufferSyncState* GetSyncState()
        {
            return static_cast<BufferSyncState*>(syncState.get());
        }

    private:
        inline static std::shared_ptr<PhaseBuffer> bufferFallback;

    public:
        static void InitializeFallbackBuffer();
        static void DestroyFallbackBuffer();
        static PhaseBuffer* GetFallbackBuffer() { return bufferFallback.get(); }
    };
} // namespace Beer::Rendering
