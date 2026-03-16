#pragma once

#include "Rendering/Shader/Globals/GlobalBuffer.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"
#include "Rendering/Shader/Globals/LightingGlobals.hpp"
#include "Rendering/Uniforms/IShaderResource.hpp"
#include <memory>

namespace Beer::Rendering
{
    class ShaderGlobalsHandler
    {
    private:
        std::unique_ptr<GlobalBuffer> globalsBuffer = nullptr;
        EngineGlobals engineGlobalsData;
        LightingGlobals lightingGlobalsData;
        vk::raii::PipelineLayout globalLayout = nullptr;

    public:
        ShaderGlobalsHandler(const Core::Device* device);
        void Update();

        void Bind(vk::CommandBuffer commandBuffer) const;

        void SetTime(float time, float deltaTime);
        void SetCamera(const glm::mat4 viewMat, const glm::mat4 projMat, glm::vec3 cameraPos);
        void SetScreen(float width, float height);
        void SetMainLight(glm::vec3 position, glm::vec4 color);
        void SetAmbientLight(glm::vec4 shadowColor, glm::vec4 skyColor);
        std::vector<vk::DescriptorSetLayout> GetLayouts() const;

    private:
        std::vector<IShaderResource*> GetGlobalResources() const;
        std::vector<vk::DescriptorSet> GetGlobalSets() const;
    };
} // namespace Beer::Rendering
