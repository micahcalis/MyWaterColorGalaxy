#pragma once

#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Material/IDirtyTracker.hpp"
#include "Rendering/Material/IReflectedContext.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include <memory>

namespace Beer::Rendering
{
    class Material : public IReflectedContext
        , public IDirtyTracker<Material>
    {
    private:
        std::shared_ptr<Shader> shader = nullptr;
        std::unordered_map<std::string, uint32_t> dirtyTextureCounts;
        std::unordered_map<std::string, uint32_t> dirtyBufferCounts;

    public:
        Material(std::shared_ptr<Shader> shader);
        Material(const std::string& shaderName);
        const Shader* GetShader() const { return shader.get(); }

        void Update() override;
        void SetInt(const std::string& name, uint32_t val) override;
        void SetFloat(const std::string& name, float val) override;
        void SetVector(const std::string& name, glm::vec4 val) override;
        void SetColor(const std::string& name, glm::vec4 val) override;
        void SetMatrix(const std::string& name, glm::mat4 val) override;
        void SetTexture(const std::string& name, ITexture* val, bool immediate = true) override;
        void SetStructuredBuffer(const std::string& name, PhaseBuffer* val, bool immediate = true) override;

        bool IsDirty() const override;

    private:
        void MarkTextureDirty(const std::string& name);
        void MarkBufferDirty(const std::string& name);
        bool HasDirtyTextures() const;
        bool HasDirtyBuffers() const;

    protected:
        MaterialProperties* GetProperties() override { return shader->GetProperties(); }
    };
} // namespace Beer::Rendering
