#include "Rendering/Shader/ShaderReflection.hpp"
#include "Rendering/Compute/ComputeKernel.hpp"
#include "Rendering/Shader/ShaderParseDef.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Vendor/spirv_reflect/spirv_reflect.h"
#include <fstream>
#include <stdexcept>
#include "Vendor./nlohmann/json.hpp"
#include "Rendering/Shader/VertexInput.hpp"
#include "vulkan/vulkan.hpp"
#include <algorithm>

namespace Beer::Rendering
{
    std::unordered_map<std::string, ShaderProperty> ShaderReflection::ReflectProperties(const std::vector<uint32_t> spvCode, uint32_t& propertyBufferSize)
    {
        std::unordered_map<std::string, ShaderProperty> properties;
        SpvReflectShaderModule reflectModule = InitializeReflect(spvCode);
        uint32_t setCount = 0;
        propertyBufferSize = 0;

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
                    propertyBufferSize = binding->block.padded_size;
                } else if (IsTextureBinding(binding))
                {
                    properties[binding->name] = {
                        GetTextureType(binding),
                        0,
                        0,
                        binding->binding};
                } else if (IsStructuredBufferBinding(binding))
                {
                    properties[binding->name] = {
                        GetBufferType(binding),
                        0,
                        binding->block.padded_size,
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

    std::vector<KernelSettings> ShaderReflection::ReflectKernelsJson(const std::filesystem::path& jsonPath)
    {
        std::vector<KernelSettings> kernelsSettings;

        std::ifstream file(jsonPath);

        if (!file.is_open())
            throw std::runtime_error("Failed to open Shader JSON: " + jsonPath.string());

        nlohmann::json j;
        file >> j;

        if (j.contains("Kernels"))
        {
            kernelsSettings.reserve(j["Kernels"].size());

            for (const auto& [kernelTag, kernelData] : j["Kernels"].items())
            {
                KernelSettings kernelSettings{};
                kernelSettings.Name = ShaderParseDef::GetKernelName(kernelData);
                kernelSettings.Index = ShaderParseDef::GetKernelIndex(kernelData);

                kernelsSettings.emplace_back(kernelSettings);
            }

            std::sort(kernelsSettings.begin(), kernelsSettings.end(), [](const KernelSettings& a, const KernelSettings& b) {
                return a.Index < b.Index;
            });
        }

        return kernelsSettings;
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

    PropertyType ShaderReflection::GetTextureType(SpvReflectDescriptorBinding* binding)
    {
        PropertyType propType = PropertyType::Unknown;
        switch (binding->descriptor_type)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return PropertyType::Texture2D;

        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return PropertyType::RWTexture2D;

        default:
            return PropertyType::Unknown;
        }
    }

    PropertyType ShaderReflection::GetBufferType(SpvReflectDescriptorBinding* binding)
    {
        PropertyType propType = PropertyType::Unknown;
        switch (binding->descriptor_type)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
            bool isReadOnly = false;

            if (binding->block.decoration_flags & SPV_REFLECT_DECORATION_NON_WRITABLE)
            {
                isReadOnly = true;
            }

            if (binding->resource_type & SPV_REFLECT_RESOURCE_FLAG_SRV)
            {
                isReadOnly = true;
            }

            propType = isReadOnly ? PropertyType::StructuredBuffer : PropertyType::RWStructuredBuffer;
        }
        break;

        default:
            propType = PropertyType::Unknown;
            break;
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
        return binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            || binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE
            || binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    }

    bool ShaderReflection::IsStructuredBufferBinding(SpvReflectDescriptorBinding* binding)
    {
        return binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    }

    MeshBufferType ShaderReflection::GetBufferTypeFromName(const char* nameString)
    {
        if (nameString == nullptr || nameString[0] == '\0')
        {
            throw std::runtime_error("Shader Semantic missing");
        }

        std::string_view semantic = std::string_view(nameString);

        if (semantic.find(ShaderParseDef::POS_ATTRIB) != std::string_view::npos)
            return MeshBufferType::Position;
        if (semantic.find(ShaderParseDef::NORM_ATTRIB) != std::string_view::npos)
            return MeshBufferType::Normal;
        if (semantic.find(ShaderParseDef::TANG_ATTRIB) != std::string_view::npos)
            return MeshBufferType::Tangent;
        if (semantic.find(ShaderParseDef::UV_ATTRIB) != std::string_view::npos)
            return MeshBufferType::Uv;
        if (semantic.find(ShaderParseDef::COLOR_ATTRIB) != std::string_view::npos)
            return MeshBufferType::Color;

        throw std::runtime_error("Shader Semantic unknown");
    }

    VertexInput ShaderReflection::ReflectVertexInput(const std::vector<uint32_t>& spvCode, const std::string& vertexEntryPoint)
    {
        SpvReflectShaderModule reflectModule = InitializeReflect(spvCode);
        uint32_t varCount = 0;

        spvReflectEnumerateEntryPointInputVariables(&reflectModule,
            vertexEntryPoint.c_str(),
            &varCount,
            nullptr);

        std::vector<SpvReflectInterfaceVariable*> inputVars(varCount);

        spvReflectEnumerateEntryPointInputVariables(&reflectModule,
            vertexEntryPoint.c_str(),
            &varCount,
            inputVars.data());

        std::sort(inputVars.begin(), inputVars.end(), [](SpvReflectInterfaceVariable* a, SpvReflectInterfaceVariable* b) {
            return a->location < b->location;
        });

        VertexInput reflectedData{};
        uint32_t currentBindingSlot = 0;

        for (SpvReflectInterfaceVariable* var : inputVars)
        {
            if (var->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
                continue;

            MeshBufferType bufferType = GetBufferTypeFromName(var->name);
            reflectedData.BufferOrder.AddToOrder(bufferType);

            vk::Format vkFormat = GetVkFormat(var->format);

            vk::VertexInputAttributeDescription attr{};
            attr.location = var->location;
            attr.binding = currentBindingSlot;
            attr.format = vkFormat;
            attr.offset = 0;

            reflectedData.AttributeDescs.push_back(attr);

            vk::VertexInputBindingDescription bindingDesc{};
            bindingDesc.binding = currentBindingSlot;
            bindingDesc.stride = GetFormatByteSize(vkFormat);
            bindingDesc.inputRate = vk::VertexInputRate::eVertex;

            reflectedData.BindingDescs.push_back(bindingDesc);

            currentBindingSlot++;
        }

        spvReflectDestroyShaderModule(&reflectModule);
        return reflectedData;
    }

    SpvReflectShaderModule ShaderReflection::InitializeReflect(const std::vector<uint32_t>& spvCode)
    {
        SpvReflectShaderModule reflectModule;

        SpvReflectResult result = spvReflectCreateShaderModule(
            spvCode.size() * sizeof(uint32_t),
            spvCode.data(),
            &reflectModule);

        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            throw std::runtime_error("failed to reflect shader module");
        }

        return std::move(reflectModule);
    }

    vk::Format ShaderReflection::GetVkFormat(SpvReflectFormat format)
    {
        switch (format)
        {
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT: return vk::Format::eR32G32B32A32Sfloat;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT: return vk::Format::eR32G32B32Sfloat;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT: return vk::Format::eR32G32Sfloat;
        case SPV_REFLECT_FORMAT_R32_SFLOAT: return vk::Format::eR32Sfloat;
        default: throw std::runtime_error("Unsupported vertex attribute format in SPIR-V!");
        }
    }

    uint32_t ShaderReflection::GetFormatByteSize(vk::Format format)
    {
        switch (format)
        {
        case vk::Format::eR32G32B32A32Sfloat: return 16;
        case vk::Format::eR32G32B32Sfloat: return 12;
        case vk::Format::eR32G32Sfloat: return 8;
        case vk::Format::eR32Sfloat: return 4;
        default: return 0;
        }
    }

    std::vector<vk::DescriptorSetLayoutBinding> ShaderReflection::ReflectMaterialBindings(const std::vector<uint32_t>& spvCode,
        bool isComputeShader)
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        SpvReflectShaderModule reflectModule = InitializeReflect(spvCode);
        uint32_t setCount = 0;

        spvReflectEnumerateDescriptorSets(&reflectModule, &setCount, nullptr);
        std::vector<SpvReflectDescriptorSet*> sets(setCount);
        spvReflectEnumerateDescriptorSets(&reflectModule, &setCount, sets.data());

        vk::ShaderStageFlagBits stageFlags = isComputeShader ? vk::ShaderStageFlagBits::eCompute : vk::ShaderStageFlagBits::eAllGraphics;

        for (uint32_t s = 0; s < setCount; s++)
        {
            SpvReflectDescriptorSet* set = sets[s];

            if (!IsMaterialSet(set))
                continue;

            for (uint32_t b = 0; b < set->binding_count; b++)
            {
                SpvReflectDescriptorBinding* spvBinding = set->bindings[b];

                vk::DescriptorSetLayoutBinding vkBinding{};
                vkBinding.binding = spvBinding->binding;
                vkBinding.descriptorCount = spvBinding->count;

                vkBinding.stageFlags = stageFlags;
                vkBinding.descriptorType = GetVkDescriptorType(spvBinding->descriptor_type);

                bindings.push_back(vkBinding);
            }
        }

        spvReflectDestroyShaderModule(&reflectModule);
        return bindings;
    }

    vk::DescriptorType ShaderReflection::GetVkDescriptorType(SpvReflectDescriptorType type)
    {
        switch (type)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return vk::DescriptorType::eUniformBuffer;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return vk::DescriptorType::eCombinedImageSampler;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return vk::DescriptorType::eSampledImage;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return vk::DescriptorType::eStorageImage;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return vk::DescriptorType::eStorageBuffer;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return vk::DescriptorType::eStorageBufferDynamic;
        default:
            throw std::runtime_error("Unsupported descriptor type in Material reflection!");
        }
    }
} // namespace Beer::Rendering
