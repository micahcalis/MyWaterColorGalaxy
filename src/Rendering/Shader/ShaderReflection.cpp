#include "Rendering/Shader/ShaderReflection.hpp"
#include "ShaderParseDef.hpp"
#include "ShaderPass.hpp"
#include "Vendor/spirv_reflect/spirv_reflect.h"
#include <fstream>
#include <stdexcept>
#include "Vendor./nlohmann/json.hpp"

namespace Beer::Rendering
{
    std::unordered_map<std::string, ShaderProperty> ShaderReflection::ReflectProperties(const std::vector<uint32_t> spvCode)
    {
        std::unordered_map<std::string, ShaderProperty> properties;
        SpvReflectShaderModule reflectModule;

        SpvReflectResult result = spvReflectCreateShaderModule(
            spvCode.size() * sizeof(uint32_t),
            spvCode.data(),
            &reflectModule);

        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            throw std::runtime_error("failed to reflect shader module");
        }

        uint32_t setCount = 0;
        spvReflectEnumerateDescriptorSets(&reflectModule, &setCount, nullptr);
        std::vector<SpvReflectDescriptorSet*> sets(setCount);
        spvReflectEnumerateDescriptorSets(&reflectModule, &setCount, sets.data());

        for (uint32_t s = 0; s < setCount; s++)
        {
            SpvReflectDescriptorSet* set = sets[s];

            if (!IsMaterialSet(set))
                continue;

            for (uint32_t b = 0; b < set->binding_count; b++)
            {
                SpvReflectDescriptorBinding* binding = set->bindings[b];

                if (IsCBufferBinding(binding))
                {
                    for (uint32_t m = 0; m < binding->block.member_count; ++m)
                    {
                        SpvReflectBlockVariable* member = &binding->block.members[m];
                        PropertyType propertyType = GetMemberType(member);

                        properties[member->name] = {
                            propertyType,
                            member->offset,
                            member->size,
                            binding->binding};
                    }
                } else if (IsTextureBinding(binding))
                {
                    properties[binding->name] = {
                        PropertyType::Texture2D,
                        0,
                        0,
                        binding->binding};
                }
            }
        }

        spvReflectDestroyShaderModule(&reflectModule);
        return properties;
    }

    std::vector<PassSettings> ShaderReflection::ReflectSettingsJson(const std::filesystem::path& jsonPath)
    {
        std::vector<PassSettings> passesSettings;

        std::ifstream file(jsonPath);

        if (!file.is_open())
            throw std::runtime_error("Failed to open Shader JSON: " + jsonPath.string());

        nlohmann::json j;
        file >> j;

        for (auto& [passName, passData] : j["Passes"].items())
        {
            PassSettings passSettings{};
            passSettings.Type = ShaderParseDef::GetPassType(passName);
            passSettings.Vertex = ShaderParseDef::GetVertexEntryPoint(passData);
            passSettings.Fragment = ShaderParseDef::GetFragmentEntryPoint(passData);
            passSettings.PolygonMode = ShaderParseDef::GetPolygonMode(passData);
            passSettings.CullMode = ShaderParseDef::GetCullMode(passData);

            ShaderParseDef::GetBlendMode(passData,
                passSettings.Blend,
                passSettings.SrcBlend,
                passSettings.DstBlend);

            ShaderParseDef::GetDepthMode(passData,
                passSettings.DepthTest,
                passSettings.DepthWrite,
                passSettings.CompareOp);

            passesSettings.emplace_back(passSettings);
        }

        return passesSettings;
    }

    PropertyType ShaderReflection::GetMemberType(SpvReflectBlockVariable* member)
    {
        PropertyType propType = PropertyType::Unknown;

        if (member->type_description->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
        {
            auto& numericTraits = member->type_description->traits.numeric;

            if (numericTraits.matrix.column_count > 0)
            {
                propType = PropertyType::Matrix4x4;
            } else if (numericTraits.vector.component_count == 4)
            {
                propType = PropertyType::Vector4;
            } else if (numericTraits.vector.component_count == 3)
            {
                propType = PropertyType::Vector3;
            } else if (numericTraits.vector.component_count == 2)
            {
                propType = PropertyType::Vector2;
            } else
            {
                propType = PropertyType::Float;
            }
        } else if (member->type_description->type_flags & SPV_REFLECT_TYPE_FLAG_INT)
        {
            propType = PropertyType::Int;
        }

        return propType;
    }

    bool ShaderReflection::IsMaterialSet(SpvReflectDescriptorSet* set)
    {
        return set->set == 1;
    }

    bool ShaderReflection::IsCBufferBinding(SpvReflectDescriptorBinding* binding)
    {
        return binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    bool ShaderReflection::IsTextureBinding(SpvReflectDescriptorBinding* binding)
    {
        return binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    }
} // namespace Beer::Rendering
