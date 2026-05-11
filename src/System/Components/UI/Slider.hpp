#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/Colliders/QuadColliderRegistry.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
namespace Beer::System
{
    enum class SliderType
    {
        Horizontal,
        Vertical
    };

    enum class SliderDirection
    {
        Positive,
        Negative
    };

    class Slider
    {
    public:
        BeerEvent<void(float)> OnSliderChanged;

    private:
        std::unique_ptr<QuadCollider> collider;
        Rendering::Material* spriteMaterial;
        UITransform* buttonTransform;
        float value;
        SliderType type;
        SliderDirection direction;

    public:
        Slider(UITransform* colliderTransform,
            Rendering::Material* spriteMaterial,
            UITransform* buttonTransform,
            float startValue = 0.0f,
            SliderType type = SliderType::Horizontal,
            SliderDirection direction = SliderDirection::Positive)
            : collider(QuadColliderRegistry::CreateCollider(colliderTransform)), spriteMaterial(spriteMaterial), buttonTransform(buttonTransform), value(startValue), type(type), direction(direction)
        {
            collider->SetColliderActive([this](MouseInput input) -> void { OnHover(input); });
            collider->SetColliderExit([this](MouseInput input) -> void { OnHoverExit(); });
            colliderTransform->BindChild(buttonTransform);
            buttonTransform->Anchor = AnchorMode::Center;
            buttonTransform->Pivot = AnchorMode::Center;
            UpdateButtonPosition();
        }

        void ForceUpdate(float value);

    private:
        void OnHover(MouseInput mouseInput);
        void OnHoverExit();
        void OnSelect(glm::vec2 position);
        void UpdateButtonPosition();
    };
} // namespace Beer::System
