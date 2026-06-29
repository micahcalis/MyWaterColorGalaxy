#pragma once
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "System/Components/Registry/IEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class UIEntity : public IEntity
    {
    protected:
        UITransform rootTransform;
        uint32_t dirtyFramesCountBuffer = 0;
        uint32_t screenVersion = 0;

    public:
        virtual ~UIEntity() = default;
        [[nodiscard]] UITransform* GetRootTransform() { return &rootTransform; }

        void MarkDirty()
        {
            dirtyFramesCountBuffer = Rendering::UniformDescriptor::GetFramesInFlight();
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
            bool isDirty = dirtyFramesCountBuffer > 0;
            return isDirty || screenVersion != Core::Screen::Version();
        }

        virtual void UpdateDirty()
        {
            rootTransform.HierarchalUpdate();
            dirtyFramesCountBuffer--;
            screenVersion = Core::Screen::Version();
        }
    };
} // namespace Beer::System
