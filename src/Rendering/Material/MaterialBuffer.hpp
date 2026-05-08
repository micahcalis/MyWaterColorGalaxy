#pragma once

#include "MaterialProperties.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Uniforms/IShaderResource.hpp"
#include <unordered_map>
#include <vector>
#include "Rendering/Material/MaterialData.hpp"

namespace Beer::Rendering
{
    class MaterialBuffer : public IShaderResource
    {
    private:
        std::unordered_map<std::string, ITexture*> textures;
        std::unordered_map<std::string, PhaseBuffer*> structuredBuffers;
        MaterialProperties* properties = nullptr;

    public:
        MaterialBuffer(MaterialProperties* properties);
        void Update(const MaterialData& materialData);
        void SetTexture(const std::string& name, ITexture* texture, bool immediate = true);
        void SetStructuredBuffer(const std::string& name, PhaseBuffer* buffer, bool immediate = true);
        void UpdateTextureDescriptor(const std::string& name);
        void UpdateStructuredBufferDescriptor(const std::string& name);

    protected:
        std::vector<vk::DescriptorSetLayoutBinding> GetBindings() override;

    private:
        void InitializeCBuffer();
        void InitializeTextures();
        void InitializeStructuredBuffers();
        bool HasCBuffer(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
    };
} // namespace Beer::Rendering
