#include "Rendering/Material/IReflectedContext.hpp"
#include "MaterialProperties.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"

namespace Beer::Rendering
{
    void IReflectedContext::InitializeBufferData(MaterialProperties* properties)
    {
        buffer = std::make_unique<MaterialBuffer>(properties);
        materialData = std::make_unique<MaterialData>(properties);
    }

    void IReflectedContext::SetInt(const std::string& name, uint32_t val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Int)
            return;

        materialData->SetProperty(name, val);
    }

    void IReflectedContext::SetFloat(const std::string& name, float val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Float)
            return;

        materialData->SetProperty(name, val);
    }

    void IReflectedContext::SetVector(const std::string& name, glm::vec4 val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        switch (typeFetch)
        {
        case PropertyType::Vector2:
            materialData->SetProperty(name, glm::vec2(val.x, val.y));
            break;
        case PropertyType::Vector3:
            materialData->SetProperty(name, glm::vec3(val.x, val.y, val.z));
            break;
        case PropertyType::Vector4:
            materialData->SetProperty(name, val);
            break;
        default:
            return;
        }
    }

    void IReflectedContext::SetColor(const std::string& name, glm::vec4 val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Vector4)
            return;

        materialData->SetProperty(name, val);
    }

    void IReflectedContext::SetMatrix(const std::string& name, glm::mat4 val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Matrix4x4)
            return;

        materialData->SetProperty(name, val);
    }

    void IReflectedContext::SetTexture(const std::string& name, ITexture* val, bool immediate)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Texture2D
            && typeFetch != PropertyType::RWTexture2D
            && typeFetch != PropertyType::Texture2DArray
            && typeFetch != PropertyType::RWTexture2DArray)
            return;

        buffer->SetTexture(name, val, immediate);
    }

    void IReflectedContext::SetStructuredBuffer(const std::string& name, PhaseBuffer* val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::StructuredBuffer && typeFetch != PropertyType::RWStructuredBuffer)
            return;

        buffer->SetStructuredBuffer(name, val);
    }
} // namespace Beer::Rendering
