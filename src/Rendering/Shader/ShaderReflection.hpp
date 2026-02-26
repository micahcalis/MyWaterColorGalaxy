#pragma once

#include "Rendering/Shader/ShaderPassType.hpp"
#include "Vendor/spirv_reflect/spirv_reflect.h"
#include "vulkan/vulkan.hpp"
#include <unordered_map>
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Shader/ShaderPass.hpp"

namespace Beer::Rendering
{
    class ShaderReflection
    {
    public:
        static std::unordered_map<std::string, ShaderProperty> ReflectProperties(const std::vector<uint32_t> spvCode);
        static std::unordered_map<ShaderPassType, ShaderPass> ReflectPasses(const std::vector<uint32_t> spvCode);

    private:
        static PropertyType GetMemberType(SpvReflectBlockVariable* member);
        static bool IsMaterialSet(SpvReflectDescriptorSet* set);
        static bool IsCBufferBinding(SpvReflectDescriptorBinding* binding);
        static bool IsTextureBinding(SpvReflectDescriptorBinding* binding);
    };
} // namespace Beer::Rendering
