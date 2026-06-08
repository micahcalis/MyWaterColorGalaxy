#include "Rendering/Shader/Globals/ShaderGlobalsHandler.hpp"
#include "GlobalBuffer.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"
#include "Rendering/Shader/Globals/GalaxyGlobals.hpp"
#include "Rendering/Shader/Globals/LightingGlobals.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/matrix.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>
#include <stdexcept>

namespace Beer::Rendering
{
    const int SET_INDEX = 0;
    const int MODEL_SET_INDEX = 1;

    ShaderGlobalsHandler::ShaderGlobalsHandler(const Core::Device* device)
    {
        std::vector<BufferBinding> bufferBindings;

        bufferBindings.emplace_back(BufferBinding(sizeof(EngineGlobals),
            EngineGlobals::BINDING,
            EngineGlobals::DESC_COUNT));

        bufferBindings.emplace_back(BufferBinding(sizeof(LightingGlobals),
            LightingGlobals::BINDING,
            LightingGlobals::DESC_COUNT));

        bufferBindings.emplace_back(BufferBinding(sizeof(GalaxyGlobals),
            GalaxyGlobals::BINDING,
            GalaxyGlobals::DESC_COUNT));

        globalsBuffer = std::make_unique<GlobalBuffer>(std::move(bufferBindings));
        InitializeTransformDescriptor(device);

        std::vector<vk::DescriptorSetLayout> setLayouts = GetGlobalsLayout();
        setLayouts.push_back(transformDescriptor->GetLayout());

        vk::PushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute;
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
        globalsBuffer->Update(GalaxyGlobals::BINDING, &galaxyGlobalsData);
    }

    void ShaderGlobalsHandler::Bind(CommandBuffer* commandBuffer,
        const vk::PipelineBindPoint bindPoint) const
    {
        commandBuffer->BindDescriptorSets(bindPoint,
            *globalLayout,
            SET_INDEX,
            GetGlobalSets());

        commandBuffer->BindDescriptorSets(bindPoint,
            *globalLayout,
            MODEL_SET_INDEX,
            {transformDescriptor->GetSet(UniformDescriptor::GetFrameIndex())});
    }

    void ShaderGlobalsHandler::SetTime(float time, float deltaTime)
    {
        engineGlobalsData.Time = time;
        engineGlobalsData.DeltaTime = deltaTime;
    }

    void ShaderGlobalsHandler::SetCamera(const glm::mat4 viewMat,
        const glm::mat4 projMat,
        const glm::vec3 cameraPos,
        const glm::vec3 cameraForward)
    {
        glm::mat4 viewProjMat = projMat * viewMat;
        engineGlobalsData.ViewProjMat = viewProjMat;
        engineGlobalsData.InvViewProjMat = glm::inverse(viewProjMat);

        engineGlobalsData.CameraPos = cameraPos;
        engineGlobalsData.CameraDir = cameraForward;
        engineGlobalsData.ViewMat = viewMat;
        engineGlobalsData.ViewDirMat = glm::mat4(glm::mat3(viewMat));
        engineGlobalsData.ProjMat = projMat;
        engineGlobalsData.InvProjMat = glm::inverse(projMat);
    }

    void ShaderGlobalsHandler::SetZBuffer(float nearPlane, float farPlane)
    {
        engineGlobalsData.ZBufferParams = glm::vec4(nearPlane, farPlane, 1.0f / nearPlane, 1.0f / farPlane);
    }

    void ShaderGlobalsHandler::SetScreen(float width, float height)
    {
        engineGlobalsData.ScreenParams = glm::vec4(width, height, 1.0f / width, 1.0f / height);
        engineGlobalsData.ScreenParams = glm::vec4(width, height, 1.0f / width, 1.0f / height);
        glm::mat4 proj = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

        engineGlobalsData.UIOrthoProjMat = proj;
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

    void ShaderGlobalsHandler::SetTransformBuffer(PhaseBuffer* transformBuffer)
    {
        if (transformBuffer == nullptr)
        {
            throw std::runtime_error("Trying to bind nullptr TransformBuffer");
        }

        for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
        {
            transformDescriptor->UpdateStructuredBufferInfo(i, 0, transformBuffer);
        }
    }

    void ShaderGlobalsHandler::SetGalaxyMapRect(const System::PixelRect& rect)
    {
        galaxyGlobalsData.MapBotLeft = rect.BotLeft;
        galaxyGlobalsData.MapBotRight = rect.BotRight;
        galaxyGlobalsData.MapTopLeft = rect.TopLeft;
        galaxyGlobalsData.MapTopRight = rect.TopRight;
    }

    void ShaderGlobalsHandler::SetGalaxyZoom(float zoomScale, glm::vec2 panning)
    {
        galaxyGlobalsData.MapZoomScale = zoomScale;
        galaxyGlobalsData.MapZoomPanning = panning;
    }

    std::vector<vk::DescriptorSetLayout> ShaderGlobalsHandler::GetGlobalsLayout() const
    {
        std::vector<vk::DescriptorSetLayout> layouts;
        std::vector<IShaderResource*> globalResources = GetGlobalResources();

        for (auto resource : globalResources)
        {
            layouts.emplace_back(resource->GetDescriptor()->GetLayout());
        }

        return layouts;
    }

    vk::DescriptorSetLayout ShaderGlobalsHandler::GetTransformLayout() const
    {
        return transformDescriptor->GetLayout();
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

    void ShaderGlobalsHandler::InitializeTransformDescriptor(const Core::Device* device)
    {
        vk::DescriptorSetLayoutBinding transformBinding{};
        transformBinding.binding = 0;
        transformBinding.descriptorType = vk::DescriptorType::eStorageBuffer;
        transformBinding.descriptorCount = 1;
        transformBinding.stageFlags = vk::ShaderStageFlagBits::eVertex
            | vk::ShaderStageFlagBits::eFragment
            | vk::ShaderStageFlagBits::eCompute;

        transformDescriptor = std::make_unique<UniformDescriptor>(
            std::vector<vk::DescriptorSetLayoutBinding>{transformBinding});
    }
} // namespace Beer::Rendering

