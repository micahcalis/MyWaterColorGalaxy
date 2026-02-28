#pragma once

#include "Vendor/spirv_reflect/spirv_reflect.h"
#include <unordered_map>
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
#include <filesystem>
#include <vector>
#include "Rendering/Mesh/MeshBufferType.hpp"
#include "Rendering/Shader/VertexInput.hpp"

namespace Beer::Rendering
{
    class ShaderReflection
    {
    public:
        static std::unordered_map<std::string, ShaderProperty> ReflectProperties(const std::vector<uint32_t> spvCode);
        static std::vector<PassSettings> ReflectSettingsJson(const std::filesystem::path& jsonPath);
        static VertexInput ReflectVertexInput(const std::vector<uint32_t>& spvCode, const std::string& vertexEntryPoint);

    private:
        static PropertyType GetMemberType(SpvReflectBlockVariable* member);
        static bool IsMaterialSet(SpvReflectDescriptorSet* set);
        static bool IsCBufferBinding(SpvReflectDescriptorBinding* binding);
        static bool IsTextureBinding(SpvReflectDescriptorBinding* binding);
        static MeshBufferType GetBufferTypeFromName(const char* semanticString);
        static SpvReflectShaderModule InitializeReflect(const std::vector<uint32_t>& spvCode);
        static vk::Format GetVkFormat(SpvReflectFormat format);
        static uint32_t GetFormatByteSize(vk::Format format);
    };
} // namespace Beer::Rendering
