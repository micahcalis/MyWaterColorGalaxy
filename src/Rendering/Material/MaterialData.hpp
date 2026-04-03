#pragma once

#include "MaterialProperties.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include <vector>

namespace Beer::Rendering
{
    class MaterialData
    {
    public:
        const static int SET = 2;
        const static int BINDING = 0;
        const static int DESC_COUNT = 1;

    private:
        std::vector<uint8_t> rawBuffer;
        MaterialProperties* properties;

    public:
        MaterialData(MaterialProperties* properties)
            : properties(properties)
        {
            rawBuffer.resize(properties->GetBufferSize(), 0);
        }

        const void* GetData() const { return rawBuffer.data(); }
        uint32_t GetSize() const { return static_cast<uint32_t>(rawBuffer.size()); }

        template<typename T>
        void SetProperty(const std::string& name, const T& data)
        {
            const ShaderProperty* prop = properties->GetShaderProperty(name);

            if (!prop || sizeof(T) > prop->Size)
                return;

            std::memcpy(rawBuffer.data() + prop->Offset, &data, sizeof(T));
        }

        PropertyType GetTypeByName(const std::string& name)
        {
            const ShaderProperty* prop = properties->GetShaderProperty(name);

            if (!prop)
                return PropertyType::Unknown;

            return prop->Type;
        }
    };
} // namespace Beer::Rendering
