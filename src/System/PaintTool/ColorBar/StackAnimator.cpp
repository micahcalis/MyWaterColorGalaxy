#include "System/PaintTool/ColorBar/StackAnimator.hpp"
#include "System/Base/Clock/Clock.hpp"

namespace Beer::System
{
    static const float DISTANCE_EPS = 1e-4f;

    void StackAnimator::Update(bool& isDirty)
    {
        isDirty = ApplySpread();
    }

    bool StackAnimator::ApplySpread()
    {
        float delta = Clock::DeltaTime() * animationSpeed;
        bool isDirty = false;

        for (int i = 0; i < transforms.size(); i++)
        {
            glm::vec2 targetPos = basePositions[i];

            if (selectedLayer != -1 || i >= transforms.size())
            {
                if (i < selectedLayer)
                {
                    targetPos.y -= offsetScale;
                } else if (i > selectedLayer)
                {
                    targetPos.y += offsetScale;
                }
            }

            float distanceToTarget = glm::distance(transforms[i]->Position, targetPos);

            if (distanceToTarget > DISTANCE_EPS)
            {
                transforms[i]->Position = glm::mix(transforms[i]->Position, targetPos, delta);
                isDirty = true;
            } else if (distanceToTarget > 0.0f)
            {
                transforms[i]->Position = targetPos;
                isDirty = true;
            }
        }

        return isDirty;
    }
} // namespace Beer::System
