#pragma once
#include "System/Components/Registry/IEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class UIEntity : public IEntity
    {
    protected:
        UITransform rootTransform;

    public:
        virtual ~UIEntity() = default;
        [[nodiscard]] UITransform* GetRootTransform() { return &rootTransform; }

    protected:
        UIEntity(UITransform rootTransform,
            std::unique_ptr<IRenderComponent> renderComponent,
            Layer layer = Layer::UI)
            : rootTransform(rootTransform), IEntity(std::move(renderComponent), layer)
        {
        }

        void UpdateHierarchy()
        {
            rootTransform.HierarchalUpdate();
        }
    };
} // namespace Beer::System
