#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <string>

namespace Beer::Core
{
    constexpr std::string VERT = "Vert";
    constexpr std::string FRAG = "Frag";

    struct PipelineData
    {
    public:
        std::string ShaderPath;
        std::string ShaderName;

        vk::PrimitiveTopology Topology = vk::PrimitiveTopology::eTriangleList;

        vk::CullModeFlags CullMode = vk::CullModeFlagBits::eBack;
        vk::PolygonMode PolygonMode = vk::PolygonMode::eFill;

        bool UseTransparency = false;
        bool UseDepthTest = true;
        bool UseDepthWrite = true;

    public:
        std::string GetVertexName() const;
        std::string GetFragmentName() const;
    };
} // namespace Beer::Core
