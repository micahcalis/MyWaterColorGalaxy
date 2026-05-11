#pragma once

#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Shader/Globals/GalaxyGlobals.hpp"
#include "Rendering/Shader/Globals/GlobalBuffer.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"
#include "Rendering/Shader/Globals/LightingGlobals.hpp"
#include "Rendering/Uniforms/IShaderResource.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    class CommandBuffer;
}

namespace Beer::Rendering
{
    class ShaderGlobalsHandler
    {
    private:
        std::unique_ptr<GlobalBuffer> globalsBuffer = nullptr;
        std::unique_ptr<UniformDescriptor> transformDescriptor = nullptr;
        EngineGlobals engineGlobalsData;
        LightingGlobals lightingGlobalsData;
        GalaxyGlobals galaxyGlobalsData;
        vk::raii::PipelineLayout globalLayout = nullptr;

    public:
        ShaderGlobalsHandler(const Core::Device* device);
        void Update();

        void Bind(CommandBuffer* commandBuffer, const vk::PipelineBindPoint bindPoint) const;

        void SetTime(float time, float deltaTime);

        void SetCamera(const glm::mat4 viewMat,
            const glm::mat4 projMat,
            const glm::vec3 cameraPos,
            const glm::vec3 cameraForward);

        void SetScreen(float width, float height);
        void SetZBuffer(float nearPlane, float farPlane);
        void SetMainLight(glm::vec3 position, glm::vec4 color);
        void SetAmbientLight(glm::vec4 shadowColor, glm::vec4 skyColor);
        void SetTransformBuffer(PhaseBuffer* transformBuffer);
        void SetGalaxyMapRect(const System::PixelRect& rect);
        void SetGalaxyZoom(float zoomScale, glm::vec2 panning);
        std::vector<vk::DescriptorSetLayout> GetGlobalsLayout() const;
        vk::DescriptorSetLayout GetTransformLayout() const;

    private:
        std::vector<IShaderResource*> GetGlobalResources() const;
        std::vector<vk::DescriptorSet> GetGlobalSets() const;
        void InitializeTransformDescriptor(const Core::Device* device);
    };
} // namespace Beer::Rendering
