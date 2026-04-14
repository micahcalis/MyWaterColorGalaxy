#pragma once

#include "QuadCollider.hpp"
#include "System/Components/Colliders/QuadColliderRegistry.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include <memory>

namespace Beer::System
{
    class QuadColliderManager
    {
    private:
        std::unique_ptr<QuadColliderRegistry> registry;
        Function<MouseInput> getMouseInput;
        QuadCollider* previousCollider = nullptr;

    public:
        ~QuadColliderManager()
        {
            QuadColliderRegistry::SetQuadColliderRegistry(nullptr);
        }

        QuadColliderManager(Function<MouseInput> getMouseInput);
        void Update();

    private:
        QuadCollider* GetSelectedCollider(const glm::vec2 mousePixelPos);
    };
} // namespace Beer::System
