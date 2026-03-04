#pragma once

#include "Rendering/Mesh/MeshBufferOrder.hpp"
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
        vk::raii::Pipeline Pipeline;
        PassSettings Settings;
        MeshBufferOrder BufferOrder;

    public:
        ShaderPass(vk::raii::Pipeline pipeline, const PassSettings settings, const MeshBufferOrder bufferOrder)
            : Pipeline(std::move(pipeline)), Settings(settings), BufferOrder(bufferOrder)
        {
        }
    };
} // namespace Beer::Rendering
