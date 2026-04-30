#pragma once

#include "System/Components/General/IRenderComponent.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include <memory>

namespace Beer::System
{
    class Registry;
}

namespace Beer::System
{
    class IEntity
    {
        friend class Registry;

    protected:
        uint32_t id = 0;
        std::unique_ptr<IRenderComponent> renderComponent;
        std::unique_ptr<IEntityManager> manager = nullptr;
        Layer layer = Layer::Default;
        bool enabled = true;

    public:
        virtual ~IEntity() = default;
        virtual void Update() = 0;

        [[nodiscard]] IRenderComponent* GetRenderComponent() { return renderComponent.get(); }
        void SetId(Registry* assigner, uint32_t id);
        bool IsAssigned() const;
        bool GetEnabled() const { return enabled; }
        void SetEnabled(bool enabled) { this->enabled = enabled; }

    protected:
        IEntity(std::unique_ptr<IRenderComponent> renderComponent,
            Layer layer = Layer::Default)
            : renderComponent(std::move(renderComponent)), layer(layer)
        {
        }

        virtual void InitializeManager() = 0;
    };
} // namespace Beer::System
