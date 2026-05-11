#pragma once

#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class StackAnimator
    {
    private:
        UITransform* container = nullptr;
        std::vector<UITransform*> transforms;
        std::vector<glm::vec2> basePositions;
        float animationSpeed;
        float offsetScale;

        int selectedLayer = -1;

    public:
        StackAnimator(UITransform* uiTransform,
            std::vector<UITransform*> transforms,
            float animationSpeed,
            float offsetScale)
            : transforms(transforms), animationSpeed(animationSpeed), offsetScale(offsetScale)
        {
            for (auto& transform : transforms)
            {
                basePositions.push_back(transform->Position);
            }
        }

        void Update(bool& isDirty);

        void SetSelectedLayer(int layer)
        {
            selectedLayer = layer;
        }

    private:
        bool ApplySpread();
    };
} // namespace Beer::System
