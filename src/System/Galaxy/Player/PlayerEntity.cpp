#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Player/PlayerManager.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec4 SAUCER_COLOR = glm::vec4(0.75f, 0.0f, 0.0f, 1);
    static const float SAUCER_METALLIC = 1.0f;
    static const float SAUCER_SMOOTH = 0.8f;
    static const glm::vec4 COCKPIT_COLOR = glm::vec4(1.0f, 1.0f, 0.6f, 0.5f);
    static const float COCKPIT_METALLIC = 0.0f;
    static const float COCKPIT_SMOOTH = 0.6f;
    static const float PLAYER_SCALE = 1.5f;

    PlayerEntity::PlayerEntity(Function<PlayerInput> getPlayerInput,
        Layer layer)
        : getPlayerInput(getPlayerInput), GameEntity(layer)
    {
        transform.Position = PLAYER_SETTINGS.StartPos;
        transform.Scale = glm::vec3(PLAYER_SCALE);

        playerMaterial = std::make_shared<Rendering::Material>("Galaxy/UFO");
        playerMaterial->SetColor("_SaucerColor", SAUCER_COLOR);
        playerMaterial->SetFloat("_SaucerMetallic", SAUCER_METALLIC);
        playerMaterial->SetFloat("_SaucerSmoothness", SAUCER_SMOOTH);
        playerMaterial->SetColor("_CockPitColor", COCKPIT_COLOR);
        playerMaterial->SetFloat("_CockPitMetallic", COCKPIT_METALLIC);
        playerMaterial->SetFloat("_CockPitSmoothness", COCKPIT_SMOOTH);

        playerMesh = Rendering::Mesh::Get("MDL_Cube");

        std::unique_ptr<SingleMeshRender> singleMeshRender
            = RenderRegister::CreateRenderComponent<SingleMeshRender>(
                ContextType::Galaxy,
                playerMaterial,
                playerMesh,
                &transform,
                &layer);

        this->renderComponent = (std::move(singleMeshRender));
    }

    void PlayerEntity::Update()
    {
        manager->Update();
    }

    void PlayerEntity::InitializeManager()
    {
        manager = std::make_unique<PlayerManager>(this, getPlayerInput);
    }

} // namespace Beer::System
