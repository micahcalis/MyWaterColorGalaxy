#include "Rendering/Shader/Globals/ShaderGlobalsHandler.hpp"
#include "EngineGlobalBuffer.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "glm/matrix.hpp"

namespace Beer::Rendering
{
    const int SET_INDEX = 0;

    ShaderGlobalsHandler::ShaderGlobalsHandler(const Core::Device* device)
    {
        engineGlobals = std::make_unique<EngineGlobalBuffer>();

        std::vector<vk::DescriptorSetLayout> setLayouts = GetLayouts();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        globalLayout = vk::raii::PipelineLayout(device->GetLogicalDevice(), pipelineLayoutInfo);
    }

    void ShaderGlobalsHandler::Update()
    {
        engineGlobals->Update(engineGlobalsData);
    }

    void ShaderGlobalsHandler::Bind(vk::CommandBuffer cmd) const
    {
        cmd.bindDescriptorSets(
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
        return {engineGlobals.get()};
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

