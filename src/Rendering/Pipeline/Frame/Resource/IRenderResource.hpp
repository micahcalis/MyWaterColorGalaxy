#pragma once

#include "RenderResourceType.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ISyncState.hpp"

namespace Beer::Rendering
{
    class ISyncBarrier;
}

namespace Beer::Rendering
{
    class IRenderResource
    {
    protected:
        RenderResourceType type;
        std::unique_ptr<ISyncState> syncState = nullptr;

    public:
        virtual ~IRenderResource() = default;
        virtual RenderResourceType GetType() const { return type; }
        virtual std::unique_ptr<ISyncBarrier> GetBarrier(const ResourceAction action) = 0;
    };
} // namespace Beer::Rendering
