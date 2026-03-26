#include "Rendering/Material/Material.hpp"
#include "IReflectedContext.hpp"
#include "MaterialBuffer.hpp"
#include "MaterialProperties.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    Material::~Material()
    {
        dirtyMaterialsQueue.erase(this);
    }

    Material::Material(std::shared_ptr<Shader> shader)
        : shader(shader)
    {
        InitializeBufferData(GetProperties());
    }

    Material::Material(const std::string& shaderName)
    {
        shader = Shader::Get(shaderName);
        InitializeBufferData(GetProperties());
    }

    void Material::Update()
    {
        if (dirtyFramesCount <= 0)
            return;

        buffer->Update(*materialData);
        dirtyFramesCount--;
    }

    void Material::SetInt(const std::string& name, uint32_t val)
    {
        IReflectedContext::SetInt(name, val);
        MarkDirty();
    }

    void Material::SetFloat(const std::string& name, float val)
    {
        IReflectedContext::SetFloat(name, val);
        MarkDirty();
    }

    void Material::SetVector(const std::string& name, glm::vec4 val)
    {
        IReflectedContext::SetVector(name, val);
        MarkDirty();
    }

    void Material::SetColor(const std::string& name, glm::vec4 val)
    {
        IReflectedContext::SetColor(name, val);
        MarkDirty();
    }

    void Material::SetMatrix(const std::string& name, glm::mat4 val)
    {
        IReflectedContext::SetMatrix(name, val);
        MarkDirty();
    }

    void Material::MarkDirty()
    {
        dirtyFramesCount = UniformDescriptor::GetFramesInFlight();
        dirtyMaterialsQueue.insert(this);
    }

    void Material::UpdateDirtyMaterials()
    {
        for (auto it = dirtyMaterialsQueue.begin(); it != dirtyMaterialsQueue.end();)
        {
            Material* material = *it;
            material->Update();

            if (material->dirtyFramesCount <= 0)
            {
                it = dirtyMaterialsQueue.erase(it);
            } else
            {
                ++it;
            }
        }
    }
} // namespace Beer::Rendering
