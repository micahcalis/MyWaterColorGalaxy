#pragma once

#include "Rendering/Buffer/Image.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ISyncState.hpp"

namespace Beer::Rendering
{
    struct ImageSyncState : public ISyncState
    {
    public:
        Image* Image;
        vk::ImageLayout CurrentLayout = vk::ImageLayout::eUndefined;
    };
} // namespace Beer::Rendering
