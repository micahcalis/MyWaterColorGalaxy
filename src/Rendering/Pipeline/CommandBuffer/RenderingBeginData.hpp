#pragma once

#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct RenderingBeginData
    {
        std::vector<vk::RenderingAttachmentInfo> ColorWriteTargets;
        vk::RenderingAttachmentInfo DepthWriteTarget;
        uint32_t Width = 0;
        uint32_t Height = 0;
        bool WritesToDepth = false;
        bool IsDrawPass = true;
    };
} // namespace Beer::Rendering
