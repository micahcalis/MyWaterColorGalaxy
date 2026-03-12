#pragma once

#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include <memory>

namespace Beer::System
{
    class SingleStaticEntity : public GameEntity
    {
    public:
        SingleStaticEntity(Transform transform,
            std::unique_ptr<SingleMeshRender> singleMeshRender,
            Layer layer = Layer::Default)
            : GameEntity(transform, nullptr, layer)
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
