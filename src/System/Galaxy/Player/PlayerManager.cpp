#include "System/Galaxy/Player/PlayerManager.hpp"
#include "PlayerEntity.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include <print>

namespace Beer::System
{
    PlayerManager::PlayerManager(PlayerEntity* player,
        Function<PlayerInput> getPlayerInput,
        BeerEvent<void(bool)>* onSetPhotoMode,
        Rendering::Material* playerMaterial)
        : player(player)
        , getPlayerInput(getPlayerInput)
        , onSetPhotoMode(onSetPhotoMode)
        , playerMaterial(playerMaterial)
    {
        playerController = std::make_unique<PlayerController>(player);
        cameraEntity = std::make_unique<GameSubEntity>(Transform());
        Function<float> getNormPlayerSpeed = [this]() -> float { return playerController->GetNormalizedSpeed(); };
        Function<bool> getPhotoMode = [this]() -> bool { return photoMode; };
        playerCamera = std::make_unique<PlayerCamera>(player, cameraEntity.get(), getNormPlayerSpeed, getPhotoMode);
    }

    void PlayerManager::Update()
    {
        PlayerInput input = getPlayerInput();

        if (input.PhotoTogglePressed)
        {
            photoMode = !photoMode;
            onSetPhotoMode->Invoke(photoMode);
        }

        if (photoMode)
        {
            input.IsBoosting = false;
            input.MovementVec = glm::vec2(0);
        }

        playerController->Update(input);
        playerCamera->Update(input);
        HandleFade();
    }

    void PlayerManager::HandleFade()
    {
        float oldCutoffValue = cutoffValue;
        float deltaTime = (float)Clock::DeltaTime();

        if (photoMode)
        {
            cutoffValue -= PLAYER_SETTINGS.FadeSpeed * deltaTime;
            if (cutoffValue < 0.0f)
            {
                cutoffValue = 0.0f;
            }
        } else
        {
            cutoffValue += PLAYER_SETTINGS.FadeSpeed * deltaTime;
            if (cutoffValue > 1.0f)
            {
                cutoffValue = 1.0f;
            }
        }

        if (cutoffValue != oldCutoffValue)
        {
            playerMaterial->SetFloat("_CutoffTime", cutoffValue);
        }
    }

    void PlayerManager::LoadFromSerialized(const SerializableExplorer& serializableExplorer)
    {
        glm::vec3 scaledPos = serializableExplorer.PlayerPosition * GalaxyObjectBuffer::GALAXY_POS_SCALE;
        player->GetTransform()->Position = scaledPos;

        glm::vec4 rotation = serializableExplorer.PlayerRotation;
        player->GetTransform()->Rotation = glm::quat(rotation.x, rotation.y, rotation.z, rotation.w);
    }
} // namespace Beer::System
