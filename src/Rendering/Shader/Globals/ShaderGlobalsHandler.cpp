#include "Rendering/Shader/Globals/ShaderGlobalsHandler.hpp"
#include "GlobalBuffer.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"
#include "Rendering/Shader/Globals/LightingGlobals.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "glm/matrix.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include <memory>

namespace Beer::Rendering
{
    const int SET_INDEX = 0;

    ShaderGlobalsHandler::ShaderGlobalsHandler(const Core::Device* device)
    {
        std::vector<BufferBinding> bufferBindings;

        bufferBindings.emplace_back(BufferBinding(sizeof(EngineGlobals),
            EngineGlobals::BINDING,
            EngineGlobals::DESC_COUNT));

        bufferBindings.emplace_back(BufferBinding(sizeof(LightingGlobals),
            LightingGlobals::BINDING,
            LightingGlobals::DESC_COUNT));

        globalsBuffer = std::make_unique<GlobalBuffer>(std::move(bufferBindings));

        std::vector<vk::DescriptorSetLayout> setLayouts = GetLayouts();

        vk::PushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(Rendering::ModelPush);

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        globalLayout = vk::raii::PipelineLayout(device->GetLogicalDevice(), pipelineLayoutInfo);
    }

    void ShaderGlobalsHandler::Update()
    {
        globalsBuffer->Update(EngineGlobals::BINDING, &engineGlobalsData);
        globalsBuffer->Update(LightingGlobals::BINDING, &lightingGlobalsData);
    }

    void ShaderGlobalsHandler::Bind(vk::CommandBuffer commandBuffer) const
    {
        commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            *globalLayout,
            SET_INDEX,
            GetGlobalSets(),
            nullptr);
    }

    void ShaderGlobalsHandler::SetTime(float time, float deltaTime)
    {
        engineGlobalsData.Time = time;
        engineGlobalsData.DeltaTime = deltaTime;
    }

    void ShaderGlobalsHandler::SetCamera(const glm::mat4 viewMat, const glm::mat4 projMat, glm::vec3 cameraPos)
    {
        glm::mat4 viewProjMat = projMat * viewMat;
        engineGlobalsData.ViewProjMat = viewProjMat;
        engineGlobalsData.InvViewProjMat = glm::inverse(viewProjMat);

        engineGlobalsData.CameraPos = cameraPos;
    }

    void ShaderGlobalsHandler::SetScreen(float width, float height)
    {
        engineGlobalsData.ScreenParams = glm::vec4(width, height, 1.0f / width, 1.0f / height);
    }

    void ShaderGlobalsHandler::SetMainLight(glm::vec3 position, glm::vec4 color)
    {
        lightingGlobalsData.MainLightPos = position;
        lightingGlobalsData.MainLightColor = color;
    }

    void ShaderGlobalsHandler::SetAmbientLight(glm::vec4 shadowColor, glm::vec4 skyColor)
    {
        lightingGlobalsData.ShadowColor = shadowColor;
        lightingGlobalsData.SkyColor = skyColor;
    }

    std::vector<vk::DescriptorSetLayout> ShaderGlobalsHandler::GetLayouts() const
    {
        std::vector<vk::DescriptorSetLayout> layouts;
        std::vector<IShaderResource*> globalResources = GetGlobalResources();

        for (auto resource : globalResources)
        {
            layouts.emplace_back(resource->GetDescriptor()->GetLayout());
        }

        return layouts;
    }

    std::vector<IShaderResource*> ShaderGlobalsHandler::GetGlobalResources() const
    {
        return {globalsBuffer.get()};
    }

    std::vector<vk::DescriptorSet> ShaderGlobalsHandler::GetGlobalSets() const
    {
        std::vector<vk::DescriptorSet> sets;
        std::vector<IShaderResource*> globalResources = GetGlobalResources();

        for (auto resource : globalResources)
        {
            sets.emplace_back(resource->GetDescriptor()->GetSet(
                UniformDescriptor::GetFrameIndex()));
        }

        return sets;
    }

} // namespace Beer::Rendering

