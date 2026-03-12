#pragma once

#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include "System/Delegates/Delegate.hpp"

namespace Beer::System
{
    struct PlayerEntity : public GameEntity
    {
    private:
        Function<PlayerInput> getPlayerInput;

    public:
        PlayerEntity(Transform transform,
            std::unique_ptr<SingleMeshRender> singleMeshRender,
            Function<PlayerInput> getPlayerInput,
            Layer layer = Layer::Default)
            : getPlayerInput(getPlayerInput), GameEntity(transform, nullptr, layer)
        {
            singleMeshRender->SetTransform(&this->transform);
            singleMeshRender->SetLayer(&this->layer);
            this->renderComponent = (std::move(singleMeshRender));
        }

        void Update() override;

    private:
        void InitializeManager() override;
    };
} // namespace Beer::System
