#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/Colliders/QuadColliderRegistry.hpp"

namespace Beer::System
{
    QuadCollider::~QuadCollider()
    {
        QuadColliderRegistry::DestroyCollider(this);
    }
} // namespace Beer::System
