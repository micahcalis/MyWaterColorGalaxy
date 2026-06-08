#include "Core/BeerMacros.slang"

import Core.BeerCore;

namespace Color
{
    public float4 Sample4ColorGrad(float4[4] colors, float time)
    {
        time = saturate(time);
        float scaledTime = time * 3.0f;
        int t = (int)floor(scaledTime);
        int t2 = clamp(t + 1, 0, 3);

        return lerp(colors[t], colors[t2], frac(scaledTime));
    }
}