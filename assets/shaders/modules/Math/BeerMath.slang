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

    public float Posterize(float x, int steps)
    {
        float s = rcp((float)steps);
        return ((int)(x / s)) * s;
    }

    public float Remap(float x, float2 a, float2 b)
    {
        return b.x + (x - a.x) * (b.y - b.x) / (a.y - a.x);
    }

    public float3x3 GetOrthogonalBasis(float3 normal)
    {
        float3 n = normal;
        float sign = n.z >= 0.0f ? 1.0f : -1.0f; 
        
        float a = -1.0f / (sign + n.z);
        float b = n.x * n.y * a;
        
        float3 b1 = float3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
        float3 b2 = float3(b, sign + n.y * n.y * a, -n.y);

        return float3x3(b1, b2, n);
    }

    public float SmoothedSteps(float x, float s)
    {
        s = max(s, 0.0001); 

        float f = floor(x);
        float r = round(x);
        float a = 1.0f / (2.0f * s);
        
        float G = a * (x - (r - s)) + r - 1.0;
        float c = 0.5f - s;
        float d = abs(0.5f - frac(x)); 
        
        return (d < c) ? f : G; 
    }

    public static const float ATAN5 = 1.37340076695f;

    public float JumpStep(float a, float b, float x)
    {
        float fTerm = 8.0f * (x - a) / (b - a) - 5.0f;
        float f1 = saturate(atan(fTerm) / (2.0f * ATAN5) + 0.5f);
        float f2 = fTerm / (2.0f * ATAN5) + 0.5f;
        return max(f1, f2);
    }
}