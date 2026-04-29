#include "Core/BeerMacros.slang"

module BeerMath;

namespace Math
{
    public float2 Rotate2D(float2 centeredPos, float angle)
    {
        float radians = angle * DEG2RAD;
        float s = sin(radians);
        float c = cos(radians);

        float2x2 rotMat = float2x2(c, s, -s, c);
        return mul(rotMat, centeredPos);
    }

    public float Median(float r, float g, float b) 
    {
        return max(min(r, g), min(max(r, g), b));
    }

    public float InverseLerp(float a, float b, float t)
    {
        return (t - a)/(b - a);
    }
}