#include "Core/BeerMacros.slang"

module Easings;

namespace Math
{
    public float EaseOutQuint(float x)
    {
        return 1.0f - pow(1.0f - x, 5.0f);
    }

    public float EaseInBack(float x)
    {
        const float c1 = 1.70158f;
        const float c3 = 2.70158f;

        return c3 * x * x * x - c1 * x * x;
    }

    public float EaseInOutQuint(float x)
    {
        return x < 0.5 ? 16.0f * x * x * x * x * x : 1 - pow(-2.0f * x + 2.0f, 5.0f) / 2.0f;
    }
}