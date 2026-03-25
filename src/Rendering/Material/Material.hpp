#pragma once

#include "MaterialBuffer.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include <memory>
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"
#include <unordered_set>

namespace Beer::Rendering
{
    class Material
    {
    private:
        std::shared_ptr<Shader> shader = nullptr;
        std::unique_ptr<MaterialBuffer> buffer = nullptr;
        std::unique_ptr<MaterialData> materialData = nullptr;

        uint32_t dirtyFramesCount;

    public:
        ~Material();
        Material(std::shared_ptr<Shader> shader);
        Material(const std::string& shaderName);
        const Shader* GetShader() const { return shader.get(); }
        vk::DescriptorSet GetDescriptorSet() const { return buffer->GetDescriptor()->GetSet(UniformDescriptor::GetFrameIndex()); }

        void Update();
        void SetInt(const std::string& name, uint32_t val);
        void SetFloat(const std::string& name, float val);
        void SetVector(const std::string& name, glm::vec4 val);
        void SetColor(const std::string& name, glm::vec4 val);
        void SetMatrix(const std::string& name, glm::mat4 val);
        void SetTexture(const std::string& name, std::shared_ptr<ITexture> val);

    private:
        void InitializeMaterial();
        void MarkDirty();

    private:
        static inline std::unordered_set<Material*> dirtyMaterialsQueue;

    public:
        static void UpdateDirtyMaterials();
    };
} // namespace Beer::Rendering
