#pragma once

#include "System/Galaxy/GalaxyContext.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/ComputeShader.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Context/IContext.hpp"
#include "System/Default/SingleStaticEntity.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Components/Registry/Registry.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include "System/Light/LightEntity.hpp"
#include "glm/ext/vector_float3.hpp"
#include <memory>
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"

namespace Beer::System
{
    GalaxyContext::GalaxyContext(Function<PlayerInput> getPlayerInput)
    {
        this->getPlayerInput = getPlayerInput;

        opaquePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawOpaquePass>(
            std::string(Rendering::OPAQUE_PASS));

        skyboxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawSkyboxPass>(
            std::string(Rendering::SKYBOX_PASS));
    }

    void GalaxyContext::Update()
    {
        IContext::Update();
        playerEntity->Update();
    }

    void GalaxyContext::Load()
    {
        std::shared_ptr<Rendering::ComputeShader> computeTest = Rendering::ComputeShader::Get("TestCompute");
        computeTest->PrintConfig();
        std::shared_ptr<Rendering::ComputeContext> testContext = std::make_shared<Rendering::ComputeContext>(computeTest);

        Transform lightTransform{};
        lightTransform.Position = glm::vec3(0, 1000, 100);

        mainLightEntity = registry.CreateEntity<LightEntity>(std::move(lightTransform),
            10,
            glm::vec4(1, 1, 0.8, 1),
            glm::vec4(0.2, 0.23, 0.35, 1),
            glm::vec4(0.86, 0.98, 1, 1));

        Transform playerTransform{};
        playerTransform.Position = PLAYER_SETTINGS.StartPos;

        std::shared_ptr<Rendering::Shader> shader = Rendering::Shader::Get("SphereRaymarch");
        std::shared_ptr<Rendering::Mesh> mesh = Rendering::Mesh::Get("MDL_Cube");
        std::shared_ptr<Rendering::Material> material = std::make_shared<Rendering::Material>(shader);

        material->SetColor("_BaseColor", glm::vec4(0, 0.0f, 1, 1));
        material->SetFloat("_SphereRadius", 0.5f);

        shader->PrintConfig();

        // std::unique_ptr<SingleMeshRender> renderComponent = RenderRegister::CreateRenderComponent<SingleMeshRender>(
        //     ContextType::Galaxy, material, mesh, nullptr, nullptr);

        playerEntity
            = registry.CreateEntity<PlayerEntity>(std::move(playerTransform),
                nullptr,
                getPlayerInput);

        glm::vec3 pos = glm::vec3(0);

        int res = 10;

        for (int z = 0; z < res; z++)
        {
            for (int y = 0; y < res; y++)
            {
                for (int x = 0; x < res; x++)
                {
                    Transform staticTransform{};
                    pos = glm::vec3(x, y, z) * 2.0f;
                    staticTransform.Position = pos;

                    std::shared_ptr<Rendering::Material> staticMaterial = std::make_shared<Rendering::Material>(shader);

                    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                    float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                    float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

                    float r1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.6;
                    float r2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.6;
                    float r3 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.6;
                    float r4 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.6;

                    float metallic = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                    float smoothness = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

                    int shapeIndex = glm::floor(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f);

                    staticMaterial->SetColor("_BaseColor", glm::vec4(r, g, b, 1));
                    staticMaterial->SetVector("_Random", glm::vec4(r1, r2, r3, r4));
                    staticMaterial->SetInt("_ShapeIndex", shapeIndex);
                    staticMaterial->SetFloat("_Metallic", metallic);
                    staticMaterial->SetFloat("_Smoothness", smoothness);

                    std::unique_ptr<SingleMeshRender>
                        staticRenderComp = RenderRegister::CreateRenderComponent<SingleMeshRender>(
                            ContextType::Galaxy, staticMaterial, mesh, nullptr, nullptr);

                    staticEntities.emplace_back(registry.CreateEntity<SingleStaticEntity>(std::move(staticTransform),
                        std::move(staticRenderComp)));
                }
            }
        }
    }

    std::vector<Rendering::IRenderPass*> GalaxyContext::GetRenderPasses()
    {
        return {opaquePass, skyboxPass};
    }
} // namespace Beer::System
