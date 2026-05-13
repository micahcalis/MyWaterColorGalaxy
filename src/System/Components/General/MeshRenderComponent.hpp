#pragma once

#include "System/Components/General/IRenderComponent.hpp"

namespace Beer::System
{
    class MeshRenderComponent : public IRenderComponent
    {
    public:
        virtual ~MeshRenderComponent() = default;
        virtual float GetPlanarDist(const glm::vec3 cameraPos, const glm::vec3 cameraForward) const = 0;

        RenderCompType GetType() const override
        {
            return RenderCompType::Mesh;
        }
    };
} // namespace Beer::System
