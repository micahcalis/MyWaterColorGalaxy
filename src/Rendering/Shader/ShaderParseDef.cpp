#include "Rendering/Shader/ShaderParseDef.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

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

    void ShaderParseDef::GetBlendMode(const nlohmann::basic_json<>& passData,
        bool& blendOn,
        vk::BlendFactor& srcBlend,
        vk::BlendFactor& dstBlend)
    {
        std::string toggle = passData.value(std::string(BLEND_TOGGLE), std::string(BLEND_OFF));
        blendOn = (Hash(toggle) == Hash(BLEND_ON));

        std::string src = passData.value(std::string(BLEND_SRC_MODE), std::string(BLEND_ALPHA));
        switch (Hash(src))
        {
        case Hash(BLEND_ONE): srcBlend = vk::BlendFactor::eOne; break;
        case Hash(BLEND_ADD): srcBlend = vk::BlendFactor::eOne; break;
        case Hash(BLEND_MUL): srcBlend = vk::BlendFactor::eDstColor; break;
        case Hash(BLEND_ALPHA):
        default: srcBlend = vk::BlendFactor::eSrcAlpha; break;
        }

        std::string dst = passData.value(std::string(BLEND_DST_MODE), std::string(BLEND_ALPHA));
        switch (Hash(dst))
        {
        case Hash(BLEND_ONE): dstBlend = vk::BlendFactor::eZero; break;
        case Hash(BLEND_ADD): dstBlend = vk::BlendFactor::eOne; break;
        case Hash(BLEND_MUL): dstBlend = vk::BlendFactor::eSrcColor; break;
        case Hash(BLEND_ALPHA):
        default: dstBlend = vk::BlendFactor::eOneMinusSrcAlpha; break;
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
