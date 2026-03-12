#pragma once

#include "IEntityManager.hpp"
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
    class GameEntity
    {
        friend class Registry;

    protected:
        uint32_t id = 0;
        Transform transform;
        std::unique_ptr<IRenderComponent> renderComponent;
        std::unique_ptr<IEntityManager> manager = nullptr;
        Layer layer = Layer::Default;

    public:
        virtual ~GameEntity() = default;
        virtual void Update() = 0;

        [[nodiscard]] Transform* GetTransform() { return &transform; }
        [[nodiscard]] IRenderComponent* GetRenderComponent() { return renderComponent.get(); }
        void SetId(Registry* assigner, uint32_t id);
        bool IsAssigned() const;

    protected:
        GameEntity(Transform transform, std::unique_ptr<IRenderComponent> renderComponent)
            : transform(transform), renderComponent(std::move(renderComponent))
        {
        }

        virtual void InitializeManager() = 0;
    };
} // namespace Beer::System
