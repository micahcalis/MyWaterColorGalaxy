#pragma once

#include "System/Galaxy/GalaxyContext.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Context/IContext.hpp"
#include "System/Default/SingleStaticEntity.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Components/Registry/Registry.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include "glm/ext/vector_float3.hpp"
#include <memory>
#include <print>

namespace Beer::System
{
    GalaxyContext::GalaxyContext(Function<PlayerInput> getPlayerInput)
    {
        this->getPlayerInput = getPlayerInput;
    }

    void GalaxyContext::Update()
    {
        IContext::Update();
        playerEntity->Update();
    }

    void GalaxyContext::Load()
    {
        Transform playerTransform{};
        playerTransform.Position = PLAYER_SETTINGS.StartPos;

        std::shared_ptr<Rendering::Shader> shader = Rendering::Shader::Get("HelloTriangle");
        std::shared_ptr<Rendering::Mesh> mesh = Rendering::Mesh::Get("MDL_IcoSphere2");
        ;
        std::shared_ptr<Rendering::Material> material = std::make_shared<Rendering::Material>(shader);

        material->SetColor("_BaseColor", glm::vec4(1, 0.0f, 1, 1));

        std::unique_ptr<SingleMeshRender> renderComponent = RenderRegister::CreateRenderComponent<SingleMeshRender>(
            ContextType::Galaxy, material, mesh, nullptr, nullptr);

        playerEntity = registry.CreateEntity<PlayerEntity>(std::move(playerTransform),
            std::move(renderComponent),
            getPlayerInput);

        glm::vec3 pos = glm::vec3(0);

        for (int i = 0; i < 10; i++)
        {
            Transform staticTransform{};
            pos += glm::vec3(1, 0, 0);
            staticTransform.Position = pos;

            std::unique_ptr<SingleMeshRender> staticRenderComp = RenderRegister::CreateRenderComponent<SingleMeshRender>(
                ContextType::Galaxy, material, mesh, nullptr, nullptr);

            staticEntities.emplace_back(registry.CreateEntity<SingleStaticEntity>(std::move(staticTransform),
                std::move(staticRenderComp)));
        }
    }
} // namespace Beer::System
