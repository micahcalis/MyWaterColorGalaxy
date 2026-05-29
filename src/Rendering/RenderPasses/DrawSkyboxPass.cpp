#include "RenderPassEvent.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Camera/Camera.hpp"

namespace Beer::Rendering
{
    static const glm::vec4 SKYBOX_COLOR = glm::vec4(0, 0, 0, 1);
    static const float GRANULATION_NOISE_INTENSITY = 0.5f;
    static const float WETNESS = 0.0f;

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
} // namespace Beer::Rendering
