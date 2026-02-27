#pragma once

#include "Rendering/Shader/ShaderPassType.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

namespace Beer::Rendering
{
    struct PassSettings
    {
        ShaderPassType Type;
        std::string Vertex;
        std::string Fragment;

        vk::PolygonMode PolygonMode = vk::PolygonMode::eFill;
        vk::CullModeFlags CullMode = vk::CullModeFlagBits::eBack;

        bool Blend = false;
        vk::BlendFactor SrcBlend = vk::BlendFactor::eSrcAlpha;
        vk::BlendFactor DstBlend = vk::BlendFactor::eOneMinusSrcAlpha;

        bool DepthTest = true;
        bool DepthWrite = true;
        vk::CompareOp CompareOp = vk::CompareOp::eLess;
    };

    struct ShaderPass
    {
    public:
        const vk::raii::Pipeline Pipeline;
        const PassSettings Settings;

    public:
        ShaderPass(vk::raii::Pipeline pipeline, const PassSettings settings)
            : Pipeline(std::move(pipeline)), Settings(settings)
        {
        }
    };
} // namespace Beer::Rendering
