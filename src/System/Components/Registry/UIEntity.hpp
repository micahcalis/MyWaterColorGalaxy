#pragma once
#include "System/Components/Registry/IEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class UIEntity : public IEntity
    {
    protected:
        UITransform rootTransform;
        bool isDirty = false;
        uint32_t screenVersion = 0;

    public:
        virtual ~UIEntity() = default;
        [[nodiscard]] UITransform* GetRootTransform() { return &rootTransform; }

        void MarkDirty()
        {
            isDirty = true;
        }

    protected:
        UIEntity(UITransform rootTransform,
            std::unique_ptr<IRenderComponent> renderComponent,
            Layer layer = Layer::UI)
            : rootTransform(rootTransform), IEntity(std::move(renderComponent), layer)
        {
            MarkDirty();
        }

        void UpdateHierarchy()
        {
            rootTransform.HierarchalUpdate();
        }

        bool NeedsUpdate() const
        {
            return isDirty || screenVersion != Core::Screen::Version();
        }
    };
} // namespace Beer::System
