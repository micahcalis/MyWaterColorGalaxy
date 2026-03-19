#pragma once

#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ISyncState.hpp"

namespace Beer::Rendering
{
    struct BufferSyncState : public ISyncState
    {
    public:
        Buffer* BufferHandle;

    public:
        BufferSyncState(Buffer* bufferHandle)
            : BufferHandle(bufferHandle)
        {
        }
    };
} // namespace Beer::Rendering
