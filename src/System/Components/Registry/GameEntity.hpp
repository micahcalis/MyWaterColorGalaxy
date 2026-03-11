#pragma once

#include "IEntityManager.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/General/RenderComponent.hpp"

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
        RenderComponent renderComponent;
        std::unique_ptr<IEntityManager> manager = nullptr;

    public:
        virtual ~GameEntity() = default;
        virtual void Update() = 0;

        [[nodiscard]] Transform* GetTransform() { return &transform; }
        [[nodiscard]] RenderComponent* GetRenderComponent() { return &renderComponent; }
        void SetId(Registry* assigner, uint32_t id);
        bool IsAssigned() const;

    protected:
        GameEntity(Transform transform, RenderComponent renderComponent)
            : transform(transform), renderComponent(renderComponent)
        {
        }

        virtual void InitializeManager() = 0;
    };
} // namespace Beer::System
