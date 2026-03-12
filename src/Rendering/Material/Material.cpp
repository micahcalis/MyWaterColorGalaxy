#include "Rendering/Material/Material.hpp"
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
        InitializeMaterial();
    }

    Material::Material(const std::string& shaderName)
    {
        shader = Shader::Get(shaderName);
        InitializeMaterial();
    }

    void Material::InitializeMaterial()
    {
        MaterialProperties* properties = shader->GetProperties();
        buffer = std::make_unique<MaterialBuffer>(properties);
        materialData = std::make_unique<MaterialData>(properties);
    }

    void Material::Update()
    {
        if (dirtyFramesCount <= 0)
            return;

        buffer->Update(*materialData);
        dirtyFramesCount--;
    }

    void Material::BindBuffer(vk::CommandBuffer commandBuffer) const
    {
        commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            shader->GetPipelineLayout(),
            MaterialData::SET,
            buffer->GetDescriptor()->GetSet(UniformDescriptor::GetFrameIndex()),
            nullptr);
    }

    void Material::SetInt(const std::string& name, uint32_t val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Int)
            return;

        materialData->SetProperty(name, val);
        MarkDirty();
    }

    void Material::SetFloat(const std::string& name, float val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Float)
            return;

        materialData->SetProperty(name, val);
        MarkDirty();
    }

    void Material::SetVector(const std::string& name, glm::vec4 val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        switch (typeFetch)
        {
        case PropertyType::Vector2:
            materialData->SetProperty(name, glm::vec2(val.x, val.y));
            MarkDirty();
            break;
        case PropertyType::Vector3:
            materialData->SetProperty(name, glm::vec3(val.x, val.y, val.z));
            MarkDirty();
            break;
        case PropertyType::Vector4:
            materialData->SetProperty(name, val);
            MarkDirty();
            break;
        default:
            return;
        }
    }

    void Material::SetColor(const std::string& name, glm::vec4 val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Vector4)
            return;

        materialData->SetProperty(name, val);
        MarkDirty();
    }

    void Material::SetMatrix(const std::string& name, glm::mat4 val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Matrix4x4)
            return;

        materialData->SetProperty(name, val);
        MarkDirty();
    }

    void Material::SetTexture(const std::string& name, std::shared_ptr<ITexture> val)
    {
        PropertyType typeFetch = materialData->GetTypeByName(name);

        if (typeFetch != PropertyType::Texture2D)
            return;

        buffer->SetTexture(name, val);
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
