#include "System/Components/Colliders/QuadColliderRegistry.hpp"
#include "QuadCollider.hpp"
#include <memory>

namespace Beer::System
{
    void QuadColliderRegistry::AddCollider(QuadCollider* collider)
    {
        colliders.push_back(collider);
    }

    void QuadColliderRegistry::RemoveFromRegister(QuadCollider* collider)
    {
        std::erase(colliders, collider);
    }

    void QuadColliderRegistry::Cleanup()
    {
        std::erase(colliders, nullptr);
    }

    std::unique_ptr<QuadCollider> QuadColliderRegistry::CreateCollider(UITransform* transform)
    {
        if (quadColliderRegistry == nullptr)
            throw std::runtime_error("Render Register not Initialized");

        std::unique_ptr<QuadCollider> colliderPtr(new QuadCollider(transform));
        quadColliderRegistry->AddCollider(colliderPtr.get());

        return colliderPtr;
    }

    void QuadColliderRegistry::DestroyCollider(QuadCollider* collider)
    {
        if (quadColliderRegistry == nullptr)
            throw std::runtime_error("Render Register not Initialized");

        quadColliderRegistry->RemoveFromRegister(collider);
    }
} // namespace Beer::System
