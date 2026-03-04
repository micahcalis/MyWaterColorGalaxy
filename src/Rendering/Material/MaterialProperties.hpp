#pragma once

#include <unordered_map>
#include <string>
#include "Rendering/Shader/ShaderProperty.hpp"
#include <print>
#include "Vendor/magic_enum/magic_enum.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    class MaterialProperties
    {
    private:
        std::unordered_map<std::string, ShaderProperty> propertyMap;
        const uint32_t propertyBufferSize;
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

    public:
        MaterialProperties(std::unordered_map<std::string, ShaderProperty> propertyMap,
            uint32_t propertyBufferSize,
            std::vector<vk::DescriptorSetLayoutBinding> bindings)
            : propertyMap(std::move(propertyMap)), propertyBufferSize(propertyBufferSize), bindings(std::move(bindings))
        {
        }

        const ShaderProperty* GetShaderProperty(const std::string& propertyName) const
        {
            auto it = propertyMap.find(propertyName);
            if (it == propertyMap.end())
            {
                return nullptr;
            }

            return &it->second;
        }

        const std::unordered_map<std::string, ShaderProperty>& GetPropertyMap() const { return propertyMap; }
        uint32_t GetBufferSize() const { return propertyBufferSize; }
        const std::vector<vk::DescriptorSetLayoutBinding>& GetBindings() const { return bindings; }

        void Print()
        {
            for (const auto& property : propertyMap)
            {
                std::println("Material Property: {} - {}",
                    property.first,
                    magic_enum::enum_name(property.second.Type));
            }
        }
    };
} // namespace Beer::Rendering
