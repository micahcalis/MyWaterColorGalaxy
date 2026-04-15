#pragma once

#include "System/Components/General/IRenderComponent.hpp"

namespace Beer::System
{
    class MeshRenderComponent : public IRenderComponent
    {
    public:
        virtual ~MeshRenderComponent() = default;

        RenderCompType GetType() const override
        {
            return RenderCompType::Mesh;
        }
    };
} // namespace Beer::System
