#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Player/PlayerManager.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec4 PLAYER_COLOR = glm::vec4(1, 0, 0, 1);

    PlayerEntity::PlayerEntity(Function<PlayerInput> getPlayerInput,
        Layer layer)
        : getPlayerInput(getPlayerInput), GameEntity(layer)
    {
        transform.Position = PLAYER_SETTINGS.StartPos;

        playerMaterial = std::make_shared<Rendering::Material>("DefaultLit");
        playerMaterial->SetColor("_BaseColor", PLAYER_COLOR);

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
