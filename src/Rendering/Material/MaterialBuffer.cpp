#include "Rendering/Material/MaterialBuffer.hpp"
#include "MaterialBuffer.hpp"
#include "MaterialData.hpp"
#include "Rendering/Texture/Texture2D.hpp"

namespace Beer::Rendering
{
    MaterialBuffer::MaterialBuffer(MaterialProperties* properties)
        : properties(properties)
    {
        InitializeCBuffer();
        InitializeTextures();
    }

    void MaterialBuffer::Update(const MaterialData& materialData)
    {
        if (buffers.size() != 0)
        {
            buffers[UniformDescriptor::GetFrameIndex()]->Upload(materialData.GetData(), materialData.GetSize());
        }
    }

    void MaterialBuffer::SetTexture(const std::string& name, std::shared_ptr<ITexture> texture)
    {
        const ShaderProperty* prop = properties->GetShaderProperty(name);

        if (!prop || prop->Type != PropertyType::Texture2D)
            return;

        textures[name] = texture;

        for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
        {
            descriptor->UpdateImageInfo(i, prop->Binding, texture.get());
        }
    }

    std::vector<vk::DescriptorSetLayoutBinding> MaterialBuffer::GetBindings()
    {
        return properties->GetBindings();
    }

    void MaterialBuffer::InitializeCBuffer()
    {
        auto bindings = GetBindings();

        descriptor = std::make_unique<UniformDescriptor>(bindings);

        if (HasCBuffer(bindings))
        {
            for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
            {
                buffers.push_back(std::make_shared<Buffer>(
                    Buffer::CreateUniform(properties->GetBufferSize())));

                descriptor->UpdateBufferInfo(i,
                    MaterialData::BINDING,
                    *buffers[i],
                    properties->GetBufferSize());
            }
        }
    }
    void MaterialBuffer::InitializeTextures()
    {
        for (const auto& [name, prop] : properties->GetPropertyMap())
        {
            if (prop.Type == PropertyType::Texture2D)
            {
                std::shared_ptr<ITexture> texToBind = Texture2D::GetFallbackTexture();

                auto it = textures.find(name);
                if (it != textures.end())
                {
                    texToBind = it->second;
                }

                for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
                {
                    descriptor->UpdateImageInfo(
                        i,
                        prop.Binding,
                        texToBind.get());
                }

                this->textures[name] = texToBind;
            }
        }
    }

    bool MaterialBuffer::HasCBuffer(const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
    {
        bool hasCBuffer = false;

        for (const auto& binding : bindings)
        {
            if (binding.descriptorType == vk::DescriptorType::eUniformBuffer)
            {
                hasCBuffer = true;
                break;
            }
        }

        return hasCBuffer;
    }
} // namespace Beer::Rendering
