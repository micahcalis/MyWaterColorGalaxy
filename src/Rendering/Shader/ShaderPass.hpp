#pragma once

#include "FragmentOutput.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "VertexInput.hpp"
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
        PassSettings Settings;
        VertexInput Input;
        FragmentTemplate FragTemplate;
        mutable std::unordered_map<FragmentOutput, vk::raii::Pipeline, FormatHasher> PipelineMap;

    public:
        ShaderPass(const PassSettings settings,
            const VertexInput vertexInput,
            const FragmentTemplate fragTemplate)
            : Settings(settings), Input(vertexInput), FragTemplate(fragTemplate)
        {
        }
    };
} // namespace Beer::Rendering
