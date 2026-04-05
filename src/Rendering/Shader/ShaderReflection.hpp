#pragma once

#include "FragmentOutput.hpp"
#include "Vendor/spirv_reflect/spirv_reflect.h"
#include <unordered_map>
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
#include <filesystem>
#include <vector>
#include "Rendering/Mesh/MeshBufferType.hpp"
#include "Rendering/Shader/VertexInput.hpp"
#include "Rendering/Compute/ComputeKernel.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class ShaderReflection
    {
    public:
        static std::unordered_map<std::string, ShaderProperty> ReflectProperties(const std::vector<uint32_t> spvCode, uint32_t& propertyBufferSize);
        static std::vector<PassSettings> ReflectSettingsJson(const std::filesystem::path& jsonPath);
        static std::vector<KernelSettings> ReflectKernelsJson(const std::filesystem::path& jsonPath);
        static VertexInput ReflectVertexInput(const std::vector<uint32_t>& spvCode, const std::string& vertexEntryPoint);
        static FragmentTemplate ReflectFragment(const std::vector<uint32_t>& spvCode, const std::string& fragmentEntryPoint);
        static std::vector<vk::DescriptorSetLayoutBinding> ReflectMaterialBindings(const std::vector<uint32_t>& spvCode,
            bool isComputeShader = false);

    private:
        static PropertyType GetMemberType(SpvReflectBlockVariable* member);
        static PropertyType GetTextureType(SpvReflectDescriptorBinding* binding);
        static PropertyType GetBufferType(SpvReflectDescriptorBinding* binding);
        static bool IsMaterialSet(SpvReflectDescriptorSet* set);
        static bool IsCBufferBinding(SpvReflectDescriptorBinding* binding);
        static bool IsTextureBinding(SpvReflectDescriptorBinding* binding);
        static bool IsStructuredBufferBinding(SpvReflectDescriptorBinding* binding);
        static MeshBufferType GetBufferTypeFromName(const char* semanticString);
        static SpvReflectShaderModule InitializeReflect(const std::vector<uint32_t>& spvCode);
        static vk::Format GetVkFormat(SpvReflectFormat format);
        static uint32_t GetFormatByteSize(vk::Format format);
        static vk::DescriptorType GetVkDescriptorType(SpvReflectDescriptorType type);
        static vk::ColorComponentFlags GetComponentMask(uint32_t componentCount);
    };
} // namespace Beer::Rendering
