#include "Rendering/Material/Material.hpp"
#include "IReflectedContext.hpp"
#include "MaterialBuffer.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
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
        if (dirtyFramesCountBuffer > 0)
        {
            buffer->Update(*materialData);
            dirtyFramesCountBuffer--;
        }

        for (auto& [name, framesLeft] : dirtyTextureCounts)
        {
            if (framesLeft > 0)
            {
                buffer->UpdateTextureDescriptor(name);
                framesLeft--;
            }
        }
    }

    void Material::SetInt(const std::string& name, uint32_t val)
    {
        IReflectedContext::SetInt(name, val);
        MarkBufferDirty();
    }

    void Material::SetFloat(const std::string& name, float val)
    {
        IReflectedContext::SetFloat(name, val);
        MarkBufferDirty();
    }

    void Material::SetVector(const std::string& name, glm::vec4 val)
    {
        IReflectedContext::SetVector(name, val);
        MarkBufferDirty();
    }

    void Material::SetColor(const std::string& name, glm::vec4 val)
    {
        IReflectedContext::SetColor(name, val);
        MarkBufferDirty();
    }

    void Material::SetMatrix(const std::string& name, glm::mat4 val)
    {
        IReflectedContext::SetMatrix(name, val);
        MarkBufferDirty();
    }

    void Material::SetTexture(const std::string& name, ITexture* val)
    {
        IReflectedContext::SetTexture(name, val);
        MarkTextureDirty(name);
    }

    void Material::MarkBufferDirty()
    {
        dirtyFramesCountBuffer = UniformDescriptor::GetFramesInFlight();
        dirtyMaterialsQueue.insert(this);
    }

    void Material::MarkTextureDirty(const std::string& name)
    {
        dirtyTextureCounts[name] = UniformDescriptor::GetFramesInFlight();
        dirtyMaterialsQueue.insert(this);
    }

    bool Material::HasDirtyTextures() const
    {
        for (const auto& dirtySet : dirtyTextureCounts)
        {
            if (dirtySet.second > 0)
                return true;
        }

        return false;
    }

    void Material::UpdateDirtyMaterials()
    {
        for (auto it = dirtyMaterialsQueue.begin(); it != dirtyMaterialsQueue.end();)
        {
            Material* material = *it;
            material->Update();

            if (material->dirtyFramesCountBuffer <= 0 && !material->HasDirtyTextures())
            {
                it = dirtyMaterialsQueue.erase(it);
            } else
            {
                ++it;
            }
        }
    }
} // namespace Beer::Rendering
