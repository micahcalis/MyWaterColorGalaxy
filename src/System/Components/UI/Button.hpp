#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/Colliders/QuadColliderRegistry.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <cstdint>
#include <memory>

namespace Beer::System
{
    enum class ButtonSpriteState : uint32_t
    {
        Default = 0,
        Hover = 1,
        Click = 2
    };

    class Button
    {
    private:
        std::unique_ptr<QuadCollider> collider;
        Rendering::Material* spriteMaterial;
        Function<void> onClick;

    public:
        Button(UITransform* transform, Rendering::Material* spriteMaterial)
            : collider(QuadColliderRegistry::CreateCollider(transform)), spriteMaterial(spriteMaterial)
        {
            collider->SetColliderActive([this](MouseInput input) -> void { OnHover(input); });
            collider->SetColliderExit([this](MouseInput input) -> void { OnHoverExit(); });
        }

        void SetOnClick(Function<void> onClick)
        {
            this->onClick = onClick;
        }

        void SetEnabled(bool enabled)
        {
            collider->SetEnabled(enabled);
        }

    private:
        void OnHover(MouseInput mouseInput);
        void OnHoverExit();
    };
} // namespace Beer::System
