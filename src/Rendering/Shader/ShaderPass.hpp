#pragma once

#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct PassSettings
    {
        vk::PolygonMode PolygonMode = vk::PolygonMode::eFill;
        vk::CullModeFlags CullMode = vk::CullModeFlagBits::eBack;

        bool Blend = false;
        vk::BlendFactor SrcBlend = vk::BlendFactor::eSrcAlpha;
        vk::BlendFactor DstBland = vk::BlendFactor::eOneMinusSrcAlpha;

        bool DepthTest = true;
        bool DepthWrite = true;
        vk::CompareOp CompareOp = vk::CompareOp::eLess;
    };

    struct ShaderPass
    {
    public:
        vk::Pipeline Pipeline;
        PassSettings Settings;
    };
} // namespace Beer::Rendering
