#pragma once

#include "MaterialProperties.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Uniforms/IShaderResource.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include "Rendering/Material/MaterialData.hpp"

namespace Beer::Rendering
{
    class MaterialBuffer : public IShaderResource
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<ITexture>> textures;
        MaterialProperties* properties = nullptr;

    public:
        MaterialBuffer(MaterialProperties* properties);
        void Update(const MaterialData& materialData);
        void SetTexture(const std::string& name, std::shared_ptr<ITexture> texture);

    protected:
        std::vector<vk::DescriptorSetLayoutBinding> GetBindings() override;

    private:
        void InitializeCBuffer();
        void InitializeTextures();
        bool HasCBuffer(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
    };
} // namespace Beer::Rendering
