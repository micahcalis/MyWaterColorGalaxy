#pragma once

#include "Rendering/Shader/ShaderPassType.hpp"
#include "Vendor/nlohmann/json.hpp"
#include "vulkan/vulkan.hpp"
#include <string_view>
#include <string>

namespace Beer::Rendering
{
    class ShaderParseDef
    {
    public:
        static constexpr std::string_view POS_ATTRIB = "positionOS";
        static constexpr std::string_view NORM_ATTRIB = "normalOS";
        static constexpr std::string_view TANG_ATTRIB = "tangentOS";
        static constexpr std::string_view UV_ATTRIB = "texCoord";
        static constexpr std::string_view COLOR_ATTRIB = "color";

    private:
        static constexpr std::string_view PASS = "Pass";
        static constexpr std::string_view VERTEX = "Vertex";
        static constexpr std::string_view FRAGMENT = "Fragment";

        static constexpr std::string_view POLY_MODE = "Polygon";
        static constexpr std::string_view POLY_FILL = "Fill";
        static constexpr std::string_view POLY_EDGE = "Line";
        static constexpr std::string_view POLY_POINT = "Point";

        static constexpr std::string_view CULL_MODE = "Cull";
        static constexpr std::string_view CULL_BACK = "Back";
        static constexpr std::string_view CULL_FRONT = "Front";
        static constexpr std::string_view CULL_NONE = "None";

        static constexpr std::string_view BLEND_TOGGLE = "Blend";
        static constexpr std::string_view BLEND_ON = "On";
        static constexpr std::string_view BLEND_OFF = "Off";

        static constexpr std::string_view BLEND_SRC_MODE = "SrcOp";
        static constexpr std::string_view BLEND_DST_MODE = "DstOp";
        static constexpr std::string_view BLEND_ALPHA = "Alpha";
        static constexpr std::string_view BLEND_ONE = "One";
        static constexpr std::string_view BLEND_ADD = "Add";
        static constexpr std::string_view BLEND_MUL = "Mul";

        static constexpr std::string_view DEPTH_TEST_TOGGLE = "ZTest";
        static constexpr std::string_view DEPTH_WRITE_TOGGLE = "ZWrite";
        static constexpr std::string_view DEPTH_ON = "On";
        static constexpr std::string_view DEPTH_OFF = "Off";

        static constexpr std::string_view DEPTH_MODE = "ZMode";
        static constexpr std::string_view DEPTH_NEVER = "Never";
        static constexpr std::string_view DEPTH_ALWAYS = "Always";
        static constexpr std::string_view DEPTH_LESS = "Closer";
        static constexpr std::string_view DEPTH_EQUAL_LESS = "ECloser";
        static constexpr std::string_view DEPTH_GREATER = "Farther";
        static constexpr std::string_view DEPTH_EQUAL_GREATER = "EFarther";

        static constexpr std::string_view PASS_OPAQUE = "Opaque";
        static constexpr std::string_view PASS_TRANSPARENT = "Transparent";
        static constexpr std::string_view PASS_SKYBOX = "Skybox";
        static constexpr std::string_view PASS_DEFERRED_SHADE = "DeferredShade";

        static constexpr std::string_view KERNEL_INDEX = "Kernel";
        static constexpr std::string_view KERNEL_NAME = "Compute";

    public:
        static std::string GetVertexEntryPoint(const nlohmann::basic_json<>& passData);
        static std::string GetFragmentEntryPoint(const nlohmann::basic_json<>& passData);
        static vk::PolygonMode GetPolygonMode(const nlohmann::basic_json<>& passData);
        static vk::CullModeFlags GetCullMode(const nlohmann::basic_json<>& passData);

        static void GetBlendMode(const nlohmann::basic_json<>& passData,
            bool& blendOn,
            vk::BlendFactor& srcBlend,
            vk::BlendFactor& dstBlend);

        static void GetDepthMode(const nlohmann::basic_json<>& passData,
            bool& depthTestOn,
            bool& depthWriteOn,
            vk::CompareOp& compareOp);

        static ShaderPassType GetPassType(const std::string& passName);

        static uint32_t GetKernelIndex(const nlohmann::basic_json<>& kernelData);
        static std::string GetKernelName(const nlohmann::basic_json<>& kernelData);

        static constexpr uint32_t Hash(std::string_view str)
        {
            uint32_t hash = 2166136261u;
            for (char c : str)
            {
                hash ^= static_cast<uint32_t>(c);
                hash *= 16777619u;
            }
            return hash;
        }
    };
} // namespace Beer::Rendering
