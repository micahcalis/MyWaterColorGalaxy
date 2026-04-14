#pragma once

#include "QuadCollider.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>
#include <vector>
namespace Beer::System
{
    class QuadColliderRegistry
    {
    private:
        std::vector<QuadCollider*> colliders;

    public:
        QuadColliderRegistry();
        void AddCollider(QuadCollider* collider);
        void RemoveFromRegister(QuadCollider* collider);
        void Cleanup();
        [[nodiscard]] std::vector<QuadCollider*>& GetColliders() { return colliders; }

    private:
        inline static QuadColliderRegistry* quadColliderRegistry = nullptr;

    public:
        static std::unique_ptr<QuadCollider> CreateCollider(UITransform* transform);
        static void DestroyCollider(QuadCollider* collider);
    };
} // namespace Beer::System
