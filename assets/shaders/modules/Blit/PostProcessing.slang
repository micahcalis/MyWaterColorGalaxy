#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Samplers;
import ColorSpace;

namespace Color
{
    public float3 ApplyVibrance(float3 rgb, float vibrance)
    {
        float luminance = Functions.Luminance(rgb);
        float saturation = Functions.Saturation(rgb);

        float vibranceVal = vibrance * (1.0f - saturation);
        return lerp(float3(luminance), rgb, 1.0f + vibranceVal);
    }
}