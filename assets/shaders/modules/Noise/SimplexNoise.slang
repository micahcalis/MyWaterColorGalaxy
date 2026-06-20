#include "Core/BeerMacros.slang"

import Core.BeerCore;

namespace Noise
{
    float3 Hash33(float3 p3)
    {
        float3 p = frac(p3 * float3(0.1031f, 0.11369f, 0.13787f));
        p += dot(p, p.yxz + 19.19f);
        return -1.0f + 2.0f * frac(float3((p.x + p.y)*p.z, (p.x+p.z)*p.y, (p.y+p.z)*p.x));
    }

    float SimplexNoise(float3 p, out float hash)
    {
        const float K1 = 0.333333333f;
        const float K2 = 0.166666667f;

        float3 i = floor(p + (p.x + p.y + p.z) * K1);
        float3 d0 = p - (i - (i.x + i.y + i.z) * K2);

        float3 e = step(float3(0.0f), d0 - d0.yzx);
        float3 i1 = e * (1.0f - e.zxy);
        float3 i2 = 1.0f - e.zxy * (1.0f - e);

        float3 d1 = d0 - (i1 - 1.0f * K2);
        float3 d2 = d0 - (i2 - 2.0f * K2);
        float3 d3 = d0 - (1.0f - 3.0f * K2);

        float4 h = max(0.6f - float4(dot(d0, d0), dot(d1, d1), dot(d2, d2), dot(d3, d3)), 0.0f);
        float4 n = h * h * h * h * float4(
            dot(d0, Hash33(i)), 
            dot(d1, Hash33(i + i1)), 
            dot(d2, Hash33(i + i2)), 
            dot(d3, Hash33(i + 1.0f)));

        hash = Hash33(i).x;

        return dot(float4(31.316f), n);
    }
}