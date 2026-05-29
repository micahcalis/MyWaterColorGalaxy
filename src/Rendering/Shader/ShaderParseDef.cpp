#include "Rendering/Shader/ShaderParseDef.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "ShaderPass.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    std::string ShaderParseDef::GetVertexEntryPoint(const nlohmann::basic_json<>& passData)
    {
        return passData.value(std::string(VERTEX), "main");
    }

    std::string ShaderParseDef::GetFragmentEntryPoint(const nlohmann::basic_json<>& passData)
    {
        return passData.value(std::string(FRAGMENT), "main");
    }

    vk::PolygonMode ShaderParseDef::GetPolygonMode(const nlohmann::basic_json<>& passData)
    {
        std::string mode = passData.value(std::string(POLY_MODE), std::string(POLY_FILL));

        switch (Hash(mode))
        {
        case Hash(POLY_EDGE): return vk::PolygonMode::eLine;
        case Hash(POLY_POINT): return vk::PolygonMode::ePoint;
        case Hash(POLY_FILL):
        default: return vk::PolygonMode::eFill;
        }
    }

    vk::CullModeFlags ShaderParseDef::GetCullMode(const nlohmann::basic_json<>& passData)
    {
        std::string mode = passData.value(std::string(CULL_MODE), std::string(CULL_BACK));

        switch (Hash(mode))
        {
        case Hash(CULL_FRONT): return vk::CullModeFlagBits::eFront;
        case Hash(CULL_NONE): return vk::CullModeFlagBits::eNone;
        case Hash(CULL_BACK):
        default: return vk::CullModeFlagBits::eBack;
        }
    }

    vk::PrimitiveTopology ShaderParseDef::GetTopologyMode(const nlohmann::basic_json<>& passData)
    {
        std::string mode = passData.value(std::string(TOPO_MODE), std::string(TOPO_TRI));

        switch (Hash(mode))
        {
        case Hash(TOPO_TRI): return vk::PrimitiveTopology::eTriangleList;
        case Hash(TOPO_LINE): return vk::PrimitiveTopology::eLineList;
        case Hash(TOPO_POINT): return vk::PrimitiveTopology::ePointList;
        default: return vk::PrimitiveTopology::eTriangleList;
        }
    }

    void ShaderParseDef::GetBlendMode(const nlohmann::basic_json<>& passData, PassSettings& settings)
    {
        std::string toggle = passData.value(std::string(BLEND_TOGGLE), std::string(BLEND_OFF));
        settings.Blend = (Hash(toggle) == Hash(BLEND_ON));

        std::string src = passData.value(std::string(BLEND_SRC_MODE), std::string(BLEND_SRC_ALPHA));
        switch (Hash(src))
        {
        case Hash(BLEND_ZERO): settings.SrcBlend = vk::BlendFactor::eZero; break;
        case Hash(BLEND_ONE): settings.SrcBlend = vk::BlendFactor::eOne; break;
        case Hash(BLEND_DST_COLOR): settings.SrcBlend = vk::BlendFactor::eDstColor; break;
        case Hash(BLEND_INV_SRC_ALPHA): settings.SrcBlend = vk::BlendFactor::eOneMinusSrcAlpha; break;
        case Hash(BLEND_SRC_ALPHA):
        default: settings.SrcBlend = vk::BlendFactor::eSrcAlpha; break;
        }

        std::string dst = passData.value(std::string(BLEND_DST_MODE), std::string(BLEND_INV_SRC_ALPHA));
        switch (Hash(dst))
        {
        case Hash(BLEND_ZERO): settings.DstBlend = vk::BlendFactor::eZero; break;
        case Hash(BLEND_ONE): settings.DstBlend = vk::BlendFactor::eOne; break;
        case Hash(BLEND_SRC_COLOR): settings.DstBlend = vk::BlendFactor::eSrcColor; break;
        case Hash(BLEND_SRC_ALPHA): settings.DstBlend = vk::BlendFactor::eSrcAlpha; break;
        case Hash(BLEND_INV_SRC_ALPHA):
        default: settings.DstBlend = vk::BlendFactor::eOneMinusSrcAlpha; break;
        }

        std::string colorOp = passData.value(std::string(COLOR_OP), std::string(OP_ADD));
        switch (Hash(colorOp))
        {
        case Hash(OP_SUBTRACT): settings.ColorOp = vk::BlendOp::eSubtract; break;
        case Hash(OP_REVERSE_SUBTRACT): settings.ColorOp = vk::BlendOp::eReverseSubtract; break;
        case Hash(OP_MIN): settings.ColorOp = vk::BlendOp::eMin; break;
        case Hash(OP_MAX): settings.ColorOp = vk::BlendOp::eMax; break;
        case Hash(OP_ADD):
        default: settings.ColorOp = vk::BlendOp::eAdd; break;
        }

        if (passData.contains(std::string(BLEND_SRC_ALPHA_MODE)) || passData.contains(std::string(BLEND_DST_ALPHA_MODE)) || passData.contains(std::string(ALPHA_OP)))
        {
            settings.HasSeparateAlphaBlend = true;

            std::string srcAlpha = passData.value(std::string(BLEND_SRC_ALPHA_MODE), std::string(BLEND_ONE));
            switch (Hash(srcAlpha))
            {
            case Hash(BLEND_ZERO): settings.SrcAlphaBlend = vk::BlendFactor::eZero; break;
            case Hash(BLEND_SRC_ALPHA): settings.SrcAlphaBlend = vk::BlendFactor::eSrcAlpha; break;
            case Hash(BLEND_INV_SRC_ALPHA): settings.SrcAlphaBlend = vk::BlendFactor::eOneMinusSrcAlpha; break;
            case Hash(BLEND_ONE):
            default: settings.SrcAlphaBlend = vk::BlendFactor::eOne; break;
            }

            std::string dstAlpha = passData.value(std::string(BLEND_DST_ALPHA_MODE), std::string(BLEND_ZERO));
            switch (Hash(dstAlpha))
            {
            case Hash(BLEND_ONE): settings.DstAlphaBlend = vk::BlendFactor::eOne; break;
            case Hash(BLEND_SRC_ALPHA): settings.DstAlphaBlend = vk::BlendFactor::eSrcAlpha; break;
            case Hash(BLEND_INV_SRC_ALPHA): settings.DstAlphaBlend = vk::BlendFactor::eOneMinusSrcAlpha; break;
            case Hash(BLEND_ZERO):
            default: settings.DstAlphaBlend = vk::BlendFactor::eZero; break;
            }

            std::string alphaOp = passData.value(std::string(ALPHA_OP), std::string(OP_ADD));
            switch (Hash(alphaOp))
            {
            case Hash(OP_SUBTRACT): settings.AlphaOp = vk::BlendOp::eSubtract; break;
            case Hash(OP_REVERSE_SUBTRACT): settings.AlphaOp = vk::BlendOp::eReverseSubtract; break;
            case Hash(OP_MIN): settings.AlphaOp = vk::BlendOp::eMin; break;
            case Hash(OP_MAX): settings.AlphaOp = vk::BlendOp::eMax; break;
            case Hash(OP_ADD):
            default: settings.AlphaOp = vk::BlendOp::eAdd; break;
            }
        } else
        {
            settings.HasSeparateAlphaBlend = false;
        }
    }

    void ShaderParseDef::GetDepthMode(const nlohmann::basic_json<>& passData,
        bool& depthTestOn,
        bool& depthWriteOn,
        vk::CompareOp& compareOp)
    {
        std::string zTest = passData.value(std::string(DEPTH_TEST_TOGGLE), std::string(DEPTH_ON));
        std::string zWrite = passData.value(std::string(DEPTH_WRITE_TOGGLE), std::string(DEPTH_ON));

        depthTestOn = (Hash(zTest) == Hash(DEPTH_ON));
        depthWriteOn = (Hash(zWrite) == Hash(DEPTH_ON));

        std::string zMode = passData.value(std::string(DEPTH_MODE), std::string(DEPTH_LESS));

        switch (Hash(zMode))
        {
        case Hash(DEPTH_NEVER): compareOp = vk::CompareOp::eNever; break;
        case Hash(DEPTH_ALWAYS): compareOp = vk::CompareOp::eAlways; break;
        case Hash(DEPTH_EQUAL_LESS): compareOp = vk::CompareOp::eLessOrEqual; break;
        case Hash(DEPTH_GREATER): compareOp = vk::CompareOp::eGreater; break;
        case Hash(DEPTH_EQUAL_GREATER): compareOp = vk::CompareOp::eGreaterOrEqual; break;
        case Hash(DEPTH_LESS):
        default: compareOp = vk::CompareOp::eLess; break;
        }
    }

    ShaderPassType ShaderParseDef::GetPassType(const std::string& passName)
    {
        switch (Hash(passName))
        {
        case Hash(PASS_OPAQUE): return ShaderPassType::Opaque;
        case Hash(PASS_TRANSPARENT): return ShaderPassType::Transparent;
        case Hash(PASS_SKYBOX): return ShaderPassType::Skybox;
        case Hash(PASS_DEFERRED_SHADE): return ShaderPassType::DeferredShade;
        case Hash(PASS_UI): return ShaderPassType::UserInterface;
        case Hash(PASS_WC_PROCESSING): return ShaderPassType::WatercolorProcessing;
        case Hash(PASS_BLITCOLOR): return ShaderPassType::BlitColor;
        default: return ShaderPassType::Unknown;
        }
    }

    uint32_t ShaderParseDef::GetKernelIndex(const nlohmann::basic_json<>& kernelData)
    {
        return kernelData.value(std::string(KERNEL_INDEX), 0);
    }

    std::string ShaderParseDef::GetKernelName(const nlohmann::basic_json<>& kernelData)
    {
        return kernelData.value(std::string(KERNEL_NAME), "compute");
    }
} // namespace Beer::Rendering
