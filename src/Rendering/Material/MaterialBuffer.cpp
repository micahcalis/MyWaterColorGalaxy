#include "Rendering/Material/MaterialBuffer.hpp"
#include "MaterialBuffer.hpp"
#include "MaterialData.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"

namespace Beer::Rendering
{
    MaterialBuffer::MaterialBuffer(MaterialProperties* properties)
        : properties(properties)
    {
        InitializeCBuffer();
        InitializeTextures();
        InitializeStructuredBuffers();
    }

    void MaterialBuffer::Update(const MaterialData& materialData)
    {
        if (buffers.size() != 0)
        {
            buffers[UniformDescriptor::GetFrameIndex()]->Upload(materialData.GetData(), materialData.GetSize());
        }
    }

    void MaterialBuffer::SetTexture(const std::string& name, ITexture* texture, bool immediate)
    {
        const ShaderProperty* prop = properties->GetShaderProperty(name);

        if (!prop
            || !(prop->Type == PropertyType::Texture2D
                || prop->Type == PropertyType::RWTexture2D
                || prop->Type == PropertyType::Texture2DArray
                || prop->Type == PropertyType::RWTexture2DArray))
            return;

        textures[name] = texture;

        if (immediate)
        {
            uint32_t currentFrame = UniformDescriptor::GetFrameIndex();
            descriptor->UpdateImageInfo(currentFrame, prop, texture);
        }
    }

    void MaterialBuffer::SetStructuredBuffer(const std::string& name, PhaseBuffer* buffer, bool immediate)
    {
        const ShaderProperty* prop = properties->GetShaderProperty(name);

        if (!prop || !(prop->Type == PropertyType::StructuredBuffer || prop->Type == PropertyType::RWStructuredBuffer))
            return;

        structuredBuffers[name] = buffer;

        if (immediate)
        {
            uint32_t currentFrame = UniformDescriptor::GetFrameIndex();
            descriptor->UpdateStructuredBufferInfo(currentFrame, prop->Binding, buffer);
        }
    }

    void MaterialBuffer::UpdateTextureDescriptor(const std::string& name)
    {
        uint32_t currentFrame = UniformDescriptor::GetFrameIndex();

        auto it = textures.find(name);
        if (it != textures.end())
        {
            const ShaderProperty* prop = properties->GetShaderProperty(name);
            if (prop)
            {
                descriptor->UpdateImageInfo(currentFrame, prop, it->second);
            }
        }
    }

    void MaterialBuffer::UpdateStructuredBufferDescriptor(const std::string& name)
    {
        uint32_t currentFrame = UniformDescriptor::GetFrameIndex();

        auto it = structuredBuffers.find(name);
        if (it != structuredBuffers.end())
        {
            const ShaderProperty* prop = properties->GetShaderProperty(name);
            if (prop)
            {
                descriptor->UpdateStructuredBufferInfo(currentFrame, prop->Binding, it->second);
            }
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
            if (prop.Type == PropertyType::Texture2D || prop.Type == PropertyType::RWTexture2D)
            {
                ITexture* texToBind = Texture2D::GetFallbackTexture().get();

                auto it = textures.find(name);
                if (it != textures.end())
                {
                    texToBind = it->second;
                }

                for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
                {
                    descriptor->UpdateImageInfo(
                        i,
                        &prop,
                        texToBind);
                }

                this->textures[name] = texToBind;
            }
        }
    }

    void MaterialBuffer::InitializeStructuredBuffers()
    {
        for (const auto& [name, prop] : properties->GetPropertyMap())
        {
            if (prop.Type == PropertyType::StructuredBuffer || prop.Type == PropertyType::RWStructuredBuffer)
            {
                PhaseBuffer* bufferToBind = PhaseBuffer::GetFallbackBuffer(prop.Dynamic);

                auto it = structuredBuffers.find(name);
                if (it != structuredBuffers.end())
                {
                    bufferToBind = it->second;
                }

                for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
                {
                    descriptor->UpdateStructuredBufferInfo(
                        i,
                        prop.Binding,
                        bufferToBind);
                }

                this->structuredBuffers[name] = bufferToBind;
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
