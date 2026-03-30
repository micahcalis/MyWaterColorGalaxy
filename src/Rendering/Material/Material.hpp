#pragma once

#include "Rendering/Material/IReflectedContext.hpp"
#include "Rendering/Shader/Shader.hpp"
#include <memory>
#include <unordered_set>

namespace Beer::Rendering
{
    class Material : public IReflectedContext
    {
    private:
        std::shared_ptr<Shader> shader = nullptr;
        uint32_t dirtyFramesCount = 0;

    public:
        ~Material();
        Material(std::shared_ptr<Shader> shader);
        Material(const std::string& shaderName);
        const Shader* GetShader() const { return shader.get(); }

        void Update() override;
        void SetInt(const std::string& name, uint32_t val) override;
        void SetFloat(const std::string& name, float val) override;
        void SetVector(const std::string& name, glm::vec4 val) override;
        void SetColor(const std::string& name, glm::vec4 val) override;
        void SetMatrix(const std::string& name, glm::mat4 val) override;

    private:
        void MarkDirty();

    protected:
        MaterialProperties* GetProperties() override { return shader->GetProperties(); }

    private:
        static inline std::unordered_set<Material*> dirtyMaterialsQueue;

    public:
        static void UpdateDirtyMaterials();
    };
} // namespace Beer::Rendering
