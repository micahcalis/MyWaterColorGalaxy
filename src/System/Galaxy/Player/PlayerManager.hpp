#pragma once

#include "PlayerCamera.hpp"
#include "Rendering/Material/Material.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Galaxy/Player/PlayerController.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"

namespace Beer::System
{
    class PlayerEntity;

    class PlayerManager : public IEntityManager
    {
    private:
        PlayerEntity* player;
        std::unique_ptr<PlayerController> playerController = nullptr;
        std::unique_ptr<GameSubEntity> cameraEntity = nullptr;
        std::unique_ptr<PlayerCamera> playerCamera = nullptr;
        Function<PlayerInput> getPlayerInput;
        BeerEvent<void(bool)>* onSetPhotoMode = nullptr;
        Rendering::Material* playerMaterial = nullptr;

        bool photoMode = false;
        float cutoffValue = 1.0f;

    public:
        PlayerManager(PlayerEntity* player,
            Function<PlayerInput> getPlayerInput,
            BeerEvent<void(bool)>* onSetPhotoMode,
            Rendering::Material* playerMaterial);

        void Update() override;
        void HandleFade();
        void LoadFromSerialized(const SerializableExplorer& serializableExplorer);
        void TogglePhotoMode();

        bool GetPhotoMode() const { return photoMode; }
    };
} // namespace Beer::System

