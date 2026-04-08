#pragma once

#include "System/Galaxy/GalaxyContext.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/ComputeShader.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/ComputeTornadoParticlesPass.hpp"
#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/RenderPasses/RenderTornadoPass.hpp"
#include "Rendering/Shader/Globals/EngineGlobals.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Rendering/Text/FontAsset.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/General/MultipleMeshRender.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Components/UI/TextRenderComponent.hpp"
#include "System/Components/UI/UIRect.hpp"
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
#include <print>
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

        deferredShadePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DeferredShadePass>(
            std::string(Rendering::DEFERRED_SHADE_PASS));

        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        defaultLitMaterial = std::make_shared<Rendering::Material>("DefaultLit");
        defaultLitMaterial->SetColor("_BaseColor", glm::vec4(0, 0, 0, 1));

        // catTexture = std::make_shared<Rendering::Texture2D>("Tex_CatAnguish");

        catLitMaterial = std::make_shared<Rendering::Material>("SphereRaymarch");
        // catLitMaterial->SetTexture("_AlbedoMap", catTexture.get());

        catLitMaterial->SetColor("_BaseColor", glm::vec4(0.5f, 0.6f, 0.2f, 1));
        catLitMaterial->SetVector("_Random", glm::vec4(0.8f));
        catLitMaterial->SetInt("_ShapeIndex", 1);
        catLitMaterial->SetFloat("_Smoothness", 0.5f);
        catLitMaterial->SetFloat("_Metalllic", 0.8f);

        computePerlinPass = Rendering::IRenderPass::FetchFromRegister<Rendering::ComputePerlinPass>(
            "Compute Perlin", 90, defaultLitMaterial.get());

        tornadoPass = Rendering::IRenderPass::FetchFromRegister<Rendering::ComputeTornadoParticlesPass>(
            "Compute Tornado", 80);

        tornadoRenderPass = Rendering::IRenderPass::FetchFromRegister<Rendering::RenderTornadoPass>(
            "Render Tornado", 400);

        mirandaSansFont = Rendering::FontAsset::Get("MirandaSans");
        fontMaterial = std::make_shared<Rendering::FontMaterial>(mirandaSansFont);

        fontMaterial->SetColor(glm::vec4(1, 0, 1, 1));
        fontMaterial->SetSize(0.06f);
    }

    void GalaxyContext::Update()
    {
        IContext::Update();

        if (playerEntity != nullptr)
        {
            playerEntity->Update();
        }

        // if (testRotationEntity != nullptr)
        // {
        //     testRotationEntity->Update();
        // }
    }

    void GalaxyContext::Load()
    {
        Transform lightTransform{};
        lightTransform.Position = glm::vec3(0, 1000, 100);

        mainLightEntity = registry.CreateEntity<LightEntity>(std::move(lightTransform),
            10,
            glm::vec4(1, 1, 0.8, 1),
            glm::vec4(0.2, 0.23, 0.35, 1),
            glm::vec4(0.86, 0.98, 1, 1));

        std::shared_ptr<Rendering::Shader> shader = Rendering::Shader::Get("SphereRaymarch");
        std::shared_ptr<Rendering::Mesh> mesh = Rendering::Mesh::Get("MDL_Cube");

        // std::unique_ptr<MultipleMeshRender> multipleMeshRender = RenderRegister::CreateRenderComponent<MultipleMeshRender>(
        //     ContextType::Galaxy, catLitMaterial, mesh, nullptr);

        // Transform rotationParent{};
        // rotationParent.Position = glm::vec3(-20, 0, 20);

        // testRotationEntity = registry.CreateEntity<MultipleContainerEntity<System::RotateEntitiesManager>>(
        //     rotationParent,
        //     std::move(multipleMeshRender),
        //     100,
        //     Layer::Default,
        //     20.0f,
        //     0.3f);

        // std::unique_ptr<SingleMeshRender> perlinRenderComp = RenderRegister::CreateRenderComponent<SingleMeshRender>(
        //     ContextType::Galaxy, defaultLitMaterial, mesh, nullptr, nullptr);

        // Transform perlinTransform{};
        // perlinTransform.Position = glm::vec3(-2, -2, -2);

        // staticEntities.emplace_back(registry.CreateEntity<SingleStaticEntity>(std::move(perlinTransform),
        //     std::move(perlinRenderComp)));

        std::unique_ptr<TextRenderComponent> textRenderComponent = RenderRegister::CreateRenderComponent<TextRenderComponent>(
            ContextType::Galaxy, fontMaterial, nullptr);

        Transform textTransform{};
        UIRect textRect{};
        textRect.Mode = Rendering::AnchorMode::Center;

        textEntity = registry.CreateEntity<TextDisplayEntity>(std::move(textTransform),
            std::move(textRect),
            std::move(textRenderComponent));

        textEntity->SetText("Max is een kleine daggoe");

        Transform playerTransform{};
        playerTransform.Position = PLAYER_SETTINGS.StartPos;

        playerEntity = registry.CreateEntity<PlayerEntity>(std::move(playerTransform),
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
        return {opaquePass, deferredShadePass, drawUIPass};
        // /return {opaquePass, skyboxPass, deferredShadePass, computePerlinPass, tornadoPass, tornadoRenderPass};
    }
} // namespace Beer::System
