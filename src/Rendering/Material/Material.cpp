#include "Rendering/Material/Material.hpp"
#include "IReflectedContext.hpp"
#include "MaterialBuffer.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include <memory>

namespace Beer::Rendering
{
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

    void Material::SetTexture(const std::string& name, ITexture* val, bool immediate)
    {
        IReflectedContext::SetTexture(name, val, immediate);
        MarkTextureDirty(name);
    }

    void Material::MarkTextureDirty(const std::string& name)
    {
        dirtyTextureCounts[name] = UniformDescriptor::GetFramesInFlight();
        dirtyQueue.insert(this);
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

    bool Material::IsDirty() const
    {
        return dirtyFramesCountBuffer > 0 || HasDirtyTextures();
    }
} // namespace Beer::Rendering
