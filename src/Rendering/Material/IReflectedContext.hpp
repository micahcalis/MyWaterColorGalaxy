#pragma once

#include "MaterialProperties.hpp"
#include "Rendering/Material/MaterialBuffer.hpp"
#include "Rendering/Material/MaterialData.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "glm/glm.hpp"

namespace Beer::Rendering
{
    class IReflectedContext
    {
    protected:
        std::unique_ptr<MaterialBuffer> buffer = nullptr;
        std::unique_ptr<MaterialData> materialData = nullptr;

    public:
        virtual ~IReflectedContext() = default;
        virtual void Update() = 0;

        virtual void SetInt(const std::string& name, uint32_t val);
        virtual void SetFloat(const std::string& name, float val);
        virtual void SetVector(const std::string& name, glm::vec4 val);
        virtual void SetColor(const std::string& name, glm::vec4 val);
        virtual void SetMatrix(const std::string& name, glm::mat4 val);
        virtual void SetTexture(const std::string& name, ITexture* val, bool immediate = true);
        virtual void SetStructuredBuffer(const std::string& name, PhaseBuffer* val, bool immediate = true);

        vk::DescriptorSet GetDescriptorSet() const { return buffer->GetDescriptor()->GetSet(UniformDescriptor::GetFrameIndex()); }
        ShaderProperty GetProperty(const std::string& name);

    protected:
        void InitializeBufferData(MaterialProperties* properties);
        virtual MaterialProperties* GetProperties() = 0;
    };
} // namespace Beer::Rendering
