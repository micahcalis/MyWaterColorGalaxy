#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include "System/Delegates/Delegate.hpp"
#include <memory>

namespace Beer::System
{
    struct PlayerEntity : public GameEntity
    {
    private:
        Function<PlayerInput> getPlayerInput = nullptr;
        std::shared_ptr<Rendering::Material> playerMaterial = nullptr;
        std::shared_ptr<Rendering::Mesh> playerMesh = nullptr;

    public:
        PlayerEntity(Function<PlayerInput> getPlayerInput,
            Layer layer = Layer::Default);

        void Update() override;

    private:
        void InitializeManager() override;
    };
} // namespace Beer::System
