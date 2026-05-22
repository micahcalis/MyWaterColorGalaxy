#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Galaxy/Player/PlayerManager.hpp"
#include <memory>

namespace Beer::System
{
    struct PlayerEntity : public GameEntity
    {
    private:
        Function<PlayerInput> getPlayerInput = nullptr;
        std::shared_ptr<Rendering::Material> playerMaterial = nullptr;
        std::shared_ptr<Rendering::Mesh> playerMesh = nullptr;
        BeerEvent<void(bool)>* onSetPhotoMode = nullptr;

    public:
        PlayerEntity(Function<PlayerInput> getPlayerInput,
            BeerEvent<void(bool)>* onSetPhotoMode,
            Layer layer = Layer::Default);

        void Update() override;

        PlayerManager* GetPlayerManager() const { return static_cast<PlayerManager*>(manager.get()); }

    private:
        void InitializeManager() override;
    };
} // namespace Beer::System
