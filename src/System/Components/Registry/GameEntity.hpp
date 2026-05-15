#pragma once

#include "System/Components/Registry/IEntity.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Drawing/Layer.hpp"
#include <memory>

namespace Beer::System
{
    class Registry;
}

namespace Beer::System
{
    class GameEntity : public IEntity
    {
        friend class Registry;

    protected:
        Transform transform{};

    public:
        virtual ~GameEntity() = default;
        virtual void Update() = 0;

        [[nodiscard]] Transform* GetTransform() { return &transform; }

    protected:
        GameEntity(Layer layer = Layer::Default)
            : IEntity(nullptr, layer)
        {
        }

        virtual void InitializeManager() = 0;
    };
} // namespace Beer::System
