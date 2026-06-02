#include "RenderPassEvent.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
#include "System/Camera/Camera.hpp"

namespace Beer::Rendering
{
    static const glm::vec4 SKYBOX_COLOR = glm::vec4(0, 0, 0, 1);
    static const float GRANULATION_NOISE_INTENSITY = 0.5f;
    static const float WETNESS = 0.0f;

    static const uint32_t NOISE_RESOLUTION = 2048;
    static const VkFormat NOISE_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;
    static const uint32_t NOISE_GROUP_SIZE = 8;
    static const uint32_t NOISE_KERNEL = 0;

    DrawSkyboxPass::DrawSkyboxPass(std::shared_ptr<Rendering::Texture3D> controlNoiseVolume)
        : controlNoiseVolume(controlNoiseVolume)
        , IRenderPass("Skybox", RenderPassEvent::SKYBOX)
    {
        auto skyboxShader = Shader::Get("Galaxy/GalaxySkybox");
        skyboxMaterial = std::make_shared<Rendering::Material>(skyboxShader);
        cubeMesh = Mesh::Get("MDL_Cube");
        skyboxTransform.Scale = glm::vec3(800, 800, 800);

        skyboxMaterial->SetColor("_SkyboxColor", SKYBOX_COLOR);
        skyboxMaterial->SetFloat("_GranulationNoiseIntensity", GRANULATION_NOISE_INTENSITY);
        skyboxMaterial->SetFloat("_Wetness", WETNESS);
        skyboxMaterial->SetTexture("_ControlNoiseVolume", controlNoiseVolume.get());

        InitializeNoiseCubemap();
    }

    void DrawSkyboxPass::OnRenderSetup(const RenderContext& context)
    {
    }

    void DrawSkyboxPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        skyboxTransform.Position = System::Camera::Main()->GetTransform()->Position;
        commandBuffer->BindModelPush(skyboxTransform.GetShaderTransform(),
            skyboxMaterial->GetShader());

        const ShaderPass* pass = skyboxMaterial->GetShader()->GetPass(ShaderPassType::Skybox);
        commandBuffer->BindShaderPass(skyboxMaterial->GetShader(), pass, context.Output);
        commandBuffer->BindMaterial(skyboxMaterial.get());
        commandBuffer->BindMesh(cubeMesh.get(), &pass->Input.BufferOrder);

        Rendering::MeshDrawInfo drawInfo = cubeMesh->GetDrawInfo();
        commandBuffer->DrawMeshSingle(drawInfo);
    }

    PassDependencyList DrawSkyboxPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_NORMAL_OFFSET),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0.5f, 0.5f}),
            static_cast<vk::Format>(GBUFFER_NORMAL_OFFSET_FORMAT)));

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_WATERCOLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(GBUFFER_WATERCOLOR_FORMAT)));

        return dependencies;
    }

    void DrawSkyboxPass::InitializeNoiseCubemap()
    {
        TextureMakeSettings makeSettings{};
        makeSettings.Width = NOISE_RESOLUTION;
        makeSettings.Height = NOISE_RESOLUTION;
        makeSettings.Depth = 6;
        makeSettings.IsCubeMap = true;
        makeSettings.Format = NOISE_FORMAT;
        makeSettings.KernelIndex = NOISE_KERNEL;
        makeSettings.GroupSizeX = NOISE_GROUP_SIZE;
        makeSettings.GroupSizeY = NOISE_GROUP_SIZE;

        noiseContext = std::make_shared<ComputeContext>("Watercolor/SkyboxNoise");

        noiseCubemap = std::make_shared<Texture2D>(Texture2D::Make(makeSettings,
            noiseContext.get()));

        skyboxMaterial->SetTexture("_Cubemapyur", noiseCubemap.get());
    }
} // namespace Beer::Rendering
