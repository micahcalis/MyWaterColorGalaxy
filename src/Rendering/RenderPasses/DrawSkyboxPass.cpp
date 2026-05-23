#include "RenderPassEvent.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Camera/Camera.hpp"

namespace Beer::Rendering
{
    DrawSkyboxPass::DrawSkyboxPass()
        : IRenderPass("Skybox", RenderPassEvent::SKYBOX)
    {
        auto skyboxShader = Shader::Get("DefaultSkybox");
        skyboxMaterial = std::make_shared<Rendering::Material>(skyboxShader);
        cubeMesh = Mesh::Get("MDL_Cube");
        skyboxTransform.Scale = glm::vec3(800, 800, 800);

        skyboxMaterial->SetFloat("_GradientExponentTop", 0.9f);
        skyboxMaterial->SetFloat("_GradientExponentBot", 2);
        skyboxMaterial->SetFloat("_SunSize", 0.001f);
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
        dependencies.AddDependency(PassDependency(VIRTUAL_MAIN_COLOR,
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        return dependencies;
    }
} // namespace Beer::Rendering
